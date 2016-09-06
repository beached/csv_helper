// The MIT License (MIT)
//
// Copyright (c) 2013-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/utility/string_ref.hpp>
#include <functional>
#include <list>
#include <string>
#include <vector>

#include <daw/daw_traits.h>
#include <daw/daw_expected.h>

#include "defs.h"
#if defined( USE_SPARSE_VECTOR ) && USE_SPARSE_VECTOR == 1
#include "sparse_vector.h"
#endif

#include "data_cell.h"
#include "data_column.h"
#include "data_types.h"

namespace daw {
	namespace data {
		struct DataTable {
			using cell_type = DataCell;
			using value_type = DataColumn < std::vector<cell_type> > ;
			//TODO static_assert(daw::traits::is_regular<value_type>::value, "DataColumn isn't regular");
			using values_type = std::vector < value_type > ;
			using reference = value_type&;
			using const_reference = const value_type&;
			using iterator = values_type::iterator;
			using const_iterator = values_type::const_iterator;
			using reverse_iterator = values_type::reverse_iterator;
			using const_reverse_iterator = values_type::const_reverse_iterator;
			using difference_type = values_type::difference_type;
			using size_type = values_type::size_type;

			DataTable( ) = default;
			DataTable( std::vector<value_type> columns );

			DataTable( DataTable const & other );
			DataTable( DataTable&& value ) noexcept;

			DataTable& operator=(DataTable const & rhs);
			DataTable& operator=(DataTable && rhs) noexcept;
			void swap( DataTable & rhs ) noexcept;

			~DataTable( ) = default;
			bool operator==(const DataTable& rhs) const = delete;

			DataTable( values_type&& columns );
			DataTable( const values_type& columns );

			reference item( size_type const & column );
			reference item( boost::string_ref column );
			const_reference item( size_type const & column ) const;
			const_reference item( boost::string_ref column ) const;

			reference operator[]( size_type const & pos );
			const_reference operator[]( size_type const & pos ) const;

			reference operator[]( boost::string_ref column );
			const_reference operator[]( boost::string_ref column ) const;

			size_type size( ) const noexcept;
			bool empty( ) const noexcept;

			size_type get_column_index( boost::string_ref col ) const;

			iterator begin( );
			iterator end( );
			const_iterator begin( ) const;
			const_iterator end( ) const;
			const_iterator cbegin( ) const;
			const_iterator cend( ) const;
			reverse_iterator rbegin( );
			reverse_iterator rend( );
			const_reverse_iterator rbegin( ) const;
			const_reverse_iterator rend( ) const;
			const_reverse_iterator crbegin( ) const;
			const_reverse_iterator crend( ) const;

			void append( value_type value );

			void erase_item( size_type const & where );

			iterator erase( iterator first );
			iterator erase( iterator first, iterator last );
			void clear( );

		private:
			values_type m_items;
		};
		//TODO static_assert(daw::traits::is_regular<DataTable>::value, "DataTable isn't regular");
		void swap( DataTable & lhs, DataTable & rhs ) noexcept;

		struct parse_csv_data_param final {
			using column_filter_t = std::function<bool( std::string const & )>;
			using progress_cb_t = std::function<void( std::string )>;
		private:
			std::string m_file_name;
			DataTable::size_type m_header_row;
			column_filter_t m_column_filter;
			progress_cb_t m_progress_cb;
		public:
			parse_csv_data_param( ) = delete;
			~parse_csv_data_param( ) = default;
			parse_csv_data_param( std::string fileName, DataTable::size_type headerRow, column_filter_t columnFilter = nullptr, progress_cb_t progressCb = nullptr );
			parse_csv_data_param( parse_csv_data_param && ) = default;
			parse_csv_data_param( parse_csv_data_param const & ) = default;
			parse_csv_data_param & operator=( parse_csv_data_param && ) = default;
			parse_csv_data_param & operator=( parse_csv_data_param const & ) = default;

			bool operator==( parse_csv_data_param const & ) const = delete;

			std::string const & file_name( ) const noexcept;
			DataTable::size_type const & header_row( ) const noexcept;
			std::function<bool( std::string const & )> const & column_filter( ) const noexcept;
			std::function<void( std::string )> const & progress_cb( ) const noexcept;
		};
		//TOOD static_assert(daw::traits::is_regular<parse_csv_data_param>::value, "parse_csv_data_param isn't regular");

		/// <summary>Parse a CSV File</summary>
		/// <param name="file_name">Path to CSV Text File with header</param>
		/// <param name="header_row">Numeric row in file that contains the header.  This will be the first line imported</param>
		/// <param name="column_filter">A function that returns true if the column name is allowed</param>
		/// <returns>A <c>DataTable</c> with the contents of the CSV File</returns>
		Expected<DataTable> parse_csv_data( const std::string &file_name, const DataTable::size_type header_row, const std::function<bool( const std::string& )> column_filter = nullptr, std::function<void( std::string )> progress_cb = nullptr );
		Expected<DataTable> parse_csv_data( const parse_csv_data_param& param );

		namespace algorithm {
			void erase_row( DataTable& table, const DataTable::size_type row );
			void erase_rows( DataTable& table, const std::vector<DataTable::size_type> rows );

			///
			// Erases Rows whenever func returns true
			///
			void erase_rows( DataTable& table, const std::function<bool( const DataTable::size_type, const DataTable& )> func );
		}
	}
}
