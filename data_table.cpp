#include <algorithm>
#include <cassert>
#include "header_libraries/algorithm.h"
#include "cstring.h"
#include "data_algorithms.h"
#include "data_cell.h"
#include "data_column.h"
#include "data_table.h"
#include "string_helpers.h"
#include "exception.h"
#include "header_libraries/expected.h"
#include "new_helper.h"
#include "header_libraries/daw_string.h"
#include "workarounds.h"
#include "memorymappedfile.h"
#include <fstream>
#include <functional>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

using daw::string::string_format;
using daw::string::string_join;

namespace daw {
	namespace data {
		namespace {
			const std::string s_empty_string = std::string( );
			/// <summary>Simple analogy used to count quotes in CSV file.  </summary>
			template<typename IntegerType>
			class CounterStack {
				static_assert(std::is_integral<IntegerType>::value, "In CounterStack IntegerType must be integral");
				IntegerType m_stack = 0;
			public:
				bool empty( ) const noexcept {
					return 0 >= m_stack;
				}

					void pop( ) {
					daw::exception::dbg_throw_on_true( empty( ), "{0}: Tried to pop an empty stack", __func__ );
					--m_stack;
				}

				void push( ) noexcept {
					++m_stack;
				}

					void reset( ) noexcept {
					m_stack = 0;
				}
			};

			class CellReference {
			private:
				size_t m_first;
				size_t m_last;
				daw::filesystem::MemoryMappedFile* m_buffer;
				bool m_empty;

				char& get( size_t pos ) {
					return (*m_buffer)[pos];
				}

				const char& get( size_t pos ) const {
					return (*m_buffer)[pos];
				}

				size_t& first( ) noexcept {
					return m_first;
				}

					size_t& last( ) noexcept {
					return m_last;
				}
				friend void trim( CellReference&, const std::string& );
				friend void clean_cell_data( CellReference&, const char );
			public:
				CellReference( daw::filesystem::MemoryMappedFile& buffer, size_t first = 0, size_t last = 0 ) : m_first( first ), m_last( last ), m_buffer( &buffer ), m_empty( true ) { }


				std::string to_string( ) const {
					if( m_empty || m_first == m_last ) {
						return s_empty_string;
					}
					const auto str_size = m_last - m_first + 1;
					return std::string( m_buffer->data( m_first ), str_size );
				}

				/// <summary>If you call this you own it or leak it</summary>
				cstring to_cstring( ) {
					if( m_last == m_first ) {
						return cstring( );
					}
					const auto str_size = m_last - m_first + 1;
					auto ptr = new_array_throw<char>( str_size + 1 );
					ptr[str_size] = 0;
					memcpy( ptr, m_buffer->data( m_first ), str_size );
					cstring result( ptr );
					result.take_ownership_of_data( );
					return std::move( result );
				}

				void clear( ) noexcept {
					m_first = m_last;
					m_empty = true;
				}

					void append( size_t pos ) noexcept {
					if( m_empty ) {
						m_first = pos;
						m_empty = false;
					}
					m_last = pos;
				}

					bool empty( ) const noexcept {
					return m_empty;
				}

					size_t size( ) const noexcept {
					return m_last - m_first + 1;
				}
			};

			inline void trim( CellReference& current_cell, const std::string& chars = " \f\n\r\t\v" ) {
				using daw::string::in;
				while( current_cell.first( ) < current_cell.last( ) && in( current_cell.get( current_cell.first( ) ), chars ) ) {
					++current_cell.first( );
				}
				if( current_cell.first( ) == current_cell.last( ) ) {
					return;
				}
				while( current_cell.first( ) < current_cell.last( ) && in( current_cell.get( current_cell.last( ) ), chars ) ) {
					--current_cell.last( );
				}
			}

			inline void clean_cell_data( CellReference& current_cell, const char string_separator ) {
				trim( current_cell );
				if( string_separator == current_cell.get( current_cell.first( ) ) && string_separator == current_cell.get( current_cell.last( ) ) ) {	// Remove any surrounding quotes as we don't need them
					++current_cell.first( );
					--current_cell.last( );
				}
			}

			template<typename FunctionType, typename FilePositionType>
			void display_progress( FunctionType progress_cb, const FilePositionType file_size, const FilePositionType file_pos, boost::posix_time::ptime start_time ) {
				const auto byte_postfix = []( double value, char& postfix ) {
					double divisor = 1.0;
					if( value >= (divisor = 1024.0*1024.0*1024.0*1024.0*1024.0) ) {
						postfix = 'P';
					} else if( value >= (divisor = 1024.0*1024.0*1024.0*1024.0) ) {
						postfix = 'T';
					} else if( value >= (divisor = 1024.0*1024.0*1024.0) ) {
						postfix = 'G';
					} else if( value >= (divisor = 1024.0*1024.0) ) {
						postfix = 'M';
					} else if( value >= (divisor = 1024.0) ) {
						postfix = 'K';
					} else {
						postfix = ' ';
					}
					value /= divisor;
					return value;
				};

				if( nullptr != progress_cb ) {	// Progress display
					const auto import_duration = boost::posix_time::second_clock::local_time( ) - start_time;
					char file_size_postfix = ' ';
					const auto file_size_fmt = byte_postfix( static_cast<double>(file_size), file_size_postfix );
					if( import_duration.total_seconds( ) >= 1 ) {
						const std::string progress = [&]( ) {
							static std::stringstream ss;
							clear( ss );
							ss.setf( std::ios::fixed, std::ios::floatfield );
							ss.fill( '0' );

							char postfix = ' ';
							const auto file_pos_fmt = byte_postfix( static_cast<double>(file_pos), postfix );
							ss.precision( 2 );
							ss << "Loading CSV Data... " << file_pos_fmt << postfix << "B of " << file_size_fmt << file_size_postfix << "B (";

							const double bps = static_cast<double>(file_pos) / static_cast<double>(import_duration.total_seconds( ));
							const double seconds_left = static_cast<real_t>(file_size - file_pos) / bps;
							const double pbps = byte_postfix( bps, postfix );
							ss.precision( 2 );
							ss << pbps;
							ss << postfix;
							ss << "B/s) ";

							const boost::posix_time::time_duration time_left = boost::posix_time::seconds( static_cast<long int>(seconds_left) );
							ss << boost::posix_time::to_simple_string( time_left );
							ss << " left";
							return ss.str( );
						}();
						progress_cb( progress );
					}
				}
			}

			/// <summary>Separate CSV File into deleniated strings</summary>
			/// <param name="fin">Input file stream of CSV File</param>
			/// <param name="header_row">Numeric row in file that contains the header.  This will be the first line imported</param>
			/// <param name="column_filter">A function that returns true if the column name is allowed</param>
			/// <returns>A <c>DataTable</c> with the contents of the CSV File</returns>
			DataTable deleniate_rows( daw::filesystem::MemoryMappedFile& buffer, const DataTable::size_type header_row, const std::function<bool( const std::string& )> column_filter, std::function<void( std::string )> progress_cb ) {
				if( !progress_cb ) {
					progress_cb = []( std::string ) { };
				}
				bool no_filter = !column_filter;
				DataTable result_datatable;

				{
					const auto file_size = static_cast<DataTable::size_type>(buffer.size( ));
					static const char delimiter = ',';
					static const char string_separator = '"';
					const auto start_time = boost::posix_time::second_clock::local_time( );
					CounterStack<DataTable::size_type> counter_stack;
					DataTable::size_type current_row_in_file = 0;
					DataTable::size_type current_column_no = 0;
					char prev_char = 0;
					DataTable::size_type file_pos = 0;
					CellReference current_cell( buffer );
					try {
						while( file_pos < file_size ) {
							const char& current_char = buffer[file_pos];

							switch( current_char ) {
							case string_separator:
								if( string_separator == prev_char ) {
									prev_char = 0;
									current_cell.append( file_pos );
									counter_stack.pop( );
								} else if( counter_stack.empty( ) ) {
									current_cell.append( file_pos );
									counter_stack.push( );
								} else {
									prev_char = 0;
									counter_stack.pop( );
								}
								break;
							case delimiter:
							case '\n':
								if( counter_stack.empty( ) ) {
									if( header_row <= current_row_in_file ) {
										// Make sure we have enough columns
										for( auto n = result_datatable.size( ); n <= current_column_no; ++n ) {
											result_datatable.append( DataTable::value_type { } );
										}
										auto& current_column = result_datatable[current_column_no];

										clean_cell_data( current_cell, string_separator );

										if( header_row == current_row_in_file ) {
											auto str = current_cell.to_string( );
											current_column.hidden( ) = no_filter ? false : !(column_filter( str ));
											current_column.header( ) = std::move( str );
										} else if( !current_column.hidden( ) ) {
											current_column.append( DataTable::cell_type::from_string( current_cell.to_cstring( ) ) );
										}
										++current_column_no;
									}
									current_cell.clear( );
									counter_stack.reset( );
									prev_char = 0;
									if( '\n' == current_char ) {
										current_column_no = 0;
										++current_row_in_file;
									}
								} else {
									current_cell.append( file_pos );
								}
								break;
							default:
								current_cell.append( file_pos );
								prev_char = current_char;
								break;
							}
							++file_pos;
#ifdef _DEBUG
							if( 0 == file_pos % 262144 ) {	// It is far slower while debugging
#else
							if( 0 == file_pos % 5242880 ) {	// It is too fast for much less.  May not actually need it
#endif
								display_progress( progress_cb, file_size, file_pos, start_time );
							}
							}		// while
						} catch( const std::exception& ex ) {
							throw ex;
						}
					}	// Scope around while with variables needed inside loop
				progress_cb( "Loading CSV Data... Processing" );
				try {
					if( !no_filter ) {
						// Remove column headers we don't want
						result_datatable.erase( std::remove_if( std::begin( result_datatable ), std::end( result_datatable ), []( const DataTable::value_type& column ) {
							return column.hidden( );
						} ), std::end( result_datatable ) );
					}

					// Verify that all columns are of equal length and append empty strings if not

					{
						const auto column_size = [&result_datatable]( ) {
							DataTable::size_type max_size = 0;
							for( auto& column : result_datatable ) {
								if( column.size( ) > max_size ) {
									max_size = column.size( );
								}
							}
							return max_size;
						}();
						for( auto& column : result_datatable ) {
							const auto num_to_add = column_size - column.size( );
							if( 0 < num_to_add ) {
								std::cerr << "Error parsing table: A column was missing " << num_to_add << " columns\n";
							}
							for( DataTable::size_type n = 0; n < num_to_add; ++n ) {
								column.append( DataTable::cell_type( ) );
							}
							column.shrink_to_fit( );
						}
					}
					return std::move( result_datatable );
				} catch( const std::exception& ex ) {
					throw ex;
				}
				}
			}

		parse_csv_data_param::parse_csv_data_param( std::string fileName, DataTable::size_type headerRow, std::function<bool( const std::string& )> columnFilter, std::function<void( std::string )> progressCb ) : m_file_name( std::move( fileName ) ), m_header_row( std::move( headerRow ) ), m_column_filter( std::move( columnFilter ) ), m_progress_cb( std::move( progressCb ) ) { }

		parse_csv_data_param::parse_csv_data_param( parse_csv_data_param&& param ) : m_file_name( std::move( param.m_file_name ) ), m_header_row( std::move( param.m_header_row ) ), m_column_filter( std::move( param.m_column_filter ) ), m_progress_cb( std::move( param.m_progress_cb ) ) { }

		parse_csv_data_param::parse_csv_data_param( const parse_csv_data_param& param ) : m_file_name( param.m_file_name ), m_header_row( param.m_header_row ), m_column_filter( param.m_column_filter ), m_progress_cb( param.m_progress_cb ) { }

		parse_csv_data_param& parse_csv_data_param::operator=(parse_csv_data_param param) noexcept {
			m_file_name = std::move( param.m_file_name );
			m_header_row = std::move( param.m_header_row );
			m_column_filter = std::move( param.m_column_filter );
			m_progress_cb = std::move( param.m_progress_cb );
			return *this;
		}

			const std::string& parse_csv_data_param::file_name( ) const noexcept {
			return m_file_name;
		}

			const DataTable::size_type& parse_csv_data_param::header_row( ) const noexcept {
			return m_header_row;
		}

			const std::function<bool( const std::string& )>& parse_csv_data_param::column_filter( ) const noexcept {
			return m_column_filter;
		}

			const std::function<void( std::string )>& parse_csv_data_param::progress_cb( ) const noexcept {
			return m_progress_cb;
		}

			Expected<DataTable> parse_csv_data( const parse_csv_data_param& param ) {
			return parse_csv_data( param.file_name( ), param.header_row( ), param.column_filter( ), param.progress_cb( ) );
		}

		Expected<DataTable> parse_csv_data( const std::string &file_name, const DataTable::size_type header_row, const std::function<bool( const std::string& )> column_filter, std::function<void( std::string )> progress_cb ) {
			std::unique_ptr<daw::filesystem::MemoryMappedFile> buffer( nullptr );
			try {
				buffer = std::make_unique<daw::filesystem::MemoryMappedFile>( file_name, true );
			} catch( const std::exception& ex ) {
				return Expected<DataTable>::from_exception( ex );
			} catch( ... ) {
				return Expected<DataTable>::from_exception( std::current_exception( ) );
			}

			if( nullptr == buffer.get( ) || !buffer->is_open( ) ) {
				return Expected<DataTable>::from_exception( std::runtime_error( string_join( __func__, ": Unrecoverable error opening file" ) ) );
			} else if( 0 >= buffer->size( ) ) {
				return Expected<DataTable>::from_exception( std::runtime_error( string_join( __func__, ": MemoryMappedFile does not have data" ) ) );
			}
			const auto start_row = header_row;	// 0 > header_row ? 0 : header_row;
			try {
				auto result = deleniate_rows( *buffer, start_row, column_filter, progress_cb );
				return std::move( result );
			} catch( const std::exception& ex ) {
				return Expected<DataTable>::from_exception( ex );
			}
		}

		// DataTable
		DataTable::DataTable( DataTable&& value ) noexcept: m_items( std::move( value.m_items ) ) { }

		DataTable& DataTable::operator=(DataTable rhs) noexcept {
			m_items = std::move( rhs.m_items );
			return *this;
		}

			DataTable::DataTable( std::vector<DataTable::value_type> columns ) : m_items( std::move( columns ) ) { }

		DataTable::DataTable( const DataTable& other ) : m_items( other.m_items ) {
			std::cerr << "DataTable copy constructor called.  Performance issue\n";
		}

		DataTable::DataTable( std::vector<DataTable::value_type>&& items ) : m_items( std::move( items ) ) { }

		DataTable::reference DataTable::operator[]( const size_t column ) {
			return item( column );
		}

		DataTable::const_reference DataTable::operator[]( const size_t column ) const {
			return item( column );
		}

		DataTable::reference DataTable::operator[]( const std::string& column ) {
			return item( column );
		}

		DataTable::const_reference DataTable::operator[]( const std::string& column ) const {
			return item( column );
		}

		DataTable::size_type DataTable::size( ) const noexcept {
			return m_items.size( );
		}

			bool DataTable::empty( ) const noexcept {
			return m_items.empty( );
		}

			const DataTable::value_type& DataTable::item( const size_type column ) const {
			return m_items[column];
		}

		DataTable::value_type& DataTable::item( const size_type column ) {
			return m_items[column];
		}

		const DataTable::value_type& DataTable::item( const std::string column ) const {
			return item( get_column_index( column ) );
		}

		DataTable::value_type& DataTable::item( const std::string column ) {
			return item( get_column_index( column ) );
		}

		DataTable::difference_type DataTable::get_column_index( const std::string column_name ) const {
			using std::begin;
			using std::end;
			using daw::algorithm::find_if;
			using namespace daw::exception;

			auto pos = find_if( m_items, [column_name]( const value_type& value ) {
				return 0 == value.header( ).compare( column_name );
			} );

			daw_throw_on_true( end( m_items ) == pos, "{0}: Could not find the column specified by name -> {1}", __func__, column_name );

			return std::distance( begin( m_items ), pos );
		}

		std::vector<DataTable::value_type>::iterator DataTable::erase( std::vector<DataTable::value_type>::iterator first ) {
			auto result = m_items.erase( first );
			return result;
		}

		std::vector<DataTable::value_type>::iterator DataTable::erase( std::vector<DataTable::value_type>::iterator first, std::vector<DataTable::value_type>::iterator last ) {
			auto result = m_items.erase( first, last );
			return result;
		}

		void DataTable::erase_item( const size_type where ) {
			daw::exception::dbg_throw_on_false( 0 <= where, "where clause to erase_item must be positive" );
			daw::exception::dbg_throw_on_false( size( ) > where, "where clause to erase_item must < size( )" );
			using daw::algorithm::begin_at;
			m_items.erase( begin_at( m_items, where ) );
		}

		DataTable::iterator DataTable::begin( ) {
			return m_items.begin( );
		}

		DataTable::iterator DataTable::end( ) {
			return m_items.end( );
		}

		DataTable::const_iterator DataTable::begin( ) const {
			return m_items.cbegin( );
		}

		DataTable::const_iterator DataTable::end( ) const {
			return m_items.cend( );
		}

		DataTable::const_iterator DataTable::cbegin( ) const {
			return m_items.cbegin( );
		}

		DataTable::const_iterator DataTable::cend( ) const {
			return m_items.cend( );
		}

		DataTable::reverse_iterator DataTable::rbegin( ) {
			return m_items.rbegin( );
		}

		DataTable::reverse_iterator DataTable::rend( ) {
			return m_items.rend( );
		}

		DataTable::const_reverse_iterator DataTable::rbegin( ) const {
			return m_items.crbegin( );
		}

		DataTable::const_reverse_iterator DataTable::rend( ) const {
			return m_items.crend( );
		}

		DataTable::const_reverse_iterator DataTable::crbegin( ) const {
			return m_items.crbegin( );
		}

		DataTable::const_reverse_iterator DataTable::crend( ) const {
			return m_items.crend( );
		}

		void DataTable::append( DataTable::value_type value ) {
			m_items.push_back( std::move( value ) );
		}


		void DataTable::clear( ) {
			// Used to assigned to value_type( ), but I cannot remember why and cannot justify it yet
			m_items.clear( );
		}
		// End DataTable

		namespace algorithm {
			void erase_row( DataTable& table, const DataTable::size_type row ) {
				parallel_for_each( table, [&row]( DataTable::reference column ) {
					erase( column, row );
				} );
			}

			void erase_rows( DataTable& table, const std::vector<DataTable::size_type> rows ) {
				parallel_for_each( table, [&rows]( DataTable::reference column ) {
					erase_items( column, rows );
				} );
			}

			void erase_rows( DataTable& table, const std::function<bool( const DataTable::size_type, const DataTable& )> func ) {
				for( ptrdiff_t n = table[0].size( ) - 1; n >= 0; --n ) {
					if( func( n, table ) ) {
						erase_row( table, n );
					}
				}
			}
		}	// namespace algorithm
		}	// namespace data
	}	// namespace daw
