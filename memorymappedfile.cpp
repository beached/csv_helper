#include <algorithm>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "header_libraries/make_unique.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>

#include "memorymappedfile.h"
#include "header_libraries/workarounds.h"

namespace daw {
	namespace filesystem {
		class MemoryMappedFilePIMPL {
		private:
			const size_t m_max_buff_size = 1048576;
			const boost::filesystem::path m_file_path;
			boost::iostreams::mapped_file_params m_mf_params;
			boost::iostreams::mapped_file m_mf_file;

			template<typename T>
			friend void std::swap( T&, T& );

		public:
			MemoryMappedFilePIMPL( const std::string &filename, const bool readonly = true ) : m_file_path( filename ), m_mf_params( filename ) {
				m_mf_params.flags = boost::iostreams::mapped_file::mapmode::readwrite;
				if( readonly ) {
					//FIXME: seems to crash	m_mf_params.flags = boost::iostreams::mapped_file::mapmode::readonly;
				}
				m_mf_params.offset = 0;
				try {
					m_mf_file.open( m_mf_params );
				} catch( const std::exception& ex ) {
					std::cerr << "Error Opening memory mapped file '" << filename << "': " << ex.what( ) << std::endl;
					throw ex;
				}
			}

			MemoryMappedFilePIMPL( ) = delete;
			MemoryMappedFilePIMPL( MemoryMappedFilePIMPL&& other ) : m_mf_params( std::move( other.m_mf_params ) ), m_mf_file( std::move( other.m_mf_file ) ) { }

			MemoryMappedFilePIMPL& operator=(MemoryMappedFilePIMPL&& rhs) {
				if( &rhs != this ) {
					MemoryMappedFilePIMPL tmp( std::move( rhs ) );
					using std::swap;
					swap( *this, tmp );
				}
				return *this;
			}

			MemoryMappedFilePIMPL( const MemoryMappedFilePIMPL& ) = delete;
			MemoryMappedFilePIMPL& operator=(const MemoryMappedFilePIMPL&) = delete;

			void close( ) {
				if( m_mf_file.is_open( ) ) {
					m_mf_file.close( );
				}
			}

			~MemoryMappedFilePIMPL( ) {
				try {
					close( );
				} catch( ... ) {
					std::cerr << "Exception while closing memory mapped file" << std::endl;
				}
			}

			bool is_open( ) const {
				return m_mf_file.is_open( );
			}

			char& operator[]( const boost::iostreams::stream_offset position ) {
				return m_mf_file.data( )[position];
			}

			const char& operator[]( const boost::iostreams::stream_offset position ) const {
				return m_mf_file.data( )[position];
			}

			char* data( const boost::iostreams::stream_offset position = 0 ) const {
				return m_mf_file.data( ) + position;
			}

			void swap( MemoryMappedFilePIMPL& other ) {
				using std::swap;
				swap( m_mf_params, other.m_mf_params );
				swap( m_mf_file, other.m_mf_file );
			}

			boost::iostreams::stream_offset size( ) const {
				return m_mf_file.size( );
			}
		};


		// MemoryMappedFile public methods
		MemoryMappedFile::MemoryMappedFile( const std::string &filename, const bool readonly ) :m_impl( daw::make_unique<MemoryMappedFilePIMPL>( filename, readonly ) ) { }

		MemoryMappedFile::MemoryMappedFile( MemoryMappedFile&& other ) noexcept: m_impl( std::move( other.m_impl ) ) { }

		MemoryMappedFile& MemoryMappedFile::operator=(MemoryMappedFile&& rhs) noexcept {
			if( &rhs != this ) {
				MemoryMappedFile tmp( std::move( rhs ) );
				using std::swap;
				swap( *this, rhs );
			}
			return *this;
		}

		MemoryMappedFile::~MemoryMappedFile( ) = default;

		void MemoryMappedFile::close( ) {
			m_impl->close( );
		}

		bool MemoryMappedFile::is_open( ) const {
			return m_impl->is_open( );
		}

		char& MemoryMappedFile::operator[]( const boost::iostreams::stream_offset position ) {
			return m_impl->operator[]( position );
		}

		const char& MemoryMappedFile::operator[]( const boost::iostreams::stream_offset position ) const {
			return m_impl->operator[]( position );
		}

		char* MemoryMappedFile::data( const boost::iostreams::stream_offset position ) const {
			return m_impl->data( position );
		}

		boost::iostreams::stream_offset MemoryMappedFile::size( ) const {
			return m_impl->size( );
		}
	}	// namespace filesystem
}	// namespace daw
