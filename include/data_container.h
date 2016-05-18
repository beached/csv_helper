#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2013-2015 Darrell Wright
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

#include "stdafx.h"
#include <iterator>
#include <map>
#include <vector>

namespace daw {
	namespace collections {
		template<typename T, typename U>
		class DataContainer {
		public:
			using value_type = T;

			using reference = value_type&;
			using const_reference = const value_type&;
			using values_type = U;

			using iterator = typename values_type::iterator;
			using const_iterator = typename values_type::const_iterator;
			using reverse_iterator = typename values_type::reverse_iterator;
			using const_reverse_iterator = typename values_type::const_reverse_iterator;
			using difference_type = typename values_type::difference_type;
			using size_type = typename values_type::size_type;

			virtual ~DataContainer( ) = 0;

			virtual typename iterator begin( ) = 0;
			virtual typename const_iterator begin( ) const = 0;
			virtual typename const_iterator cbegin( ) const = 0;

			virtual typename iterator end( ) = 0;
			virtual typename const_iterator end( ) const = 0;
			virtual typename const_iterator cend( ) const = 0;

			virtual typename size_type size( ) const = 0;
			virtual bool empty( ) const = 0;
			virtual void clear( ) = 0;

			virtual reference item( const size_type ) = 0;
			virtual const_reference item( const size_type ) const = 0;

			virtual reference operator[]( const size_type pos ) {
				return item( pos );
			}

			virtual const_reference operator[]( const size_type pos ) const {
				return item( pos );
			}
		};


		// 		template<typename T, typename U>
		// 		class DataColumn
	}	// namespace collections
	namespace data {
		class DataCell;
		class DataColumnSparse;
		namespace impl {
			template<typename T> class DataColumnSparseIterator;
			template<typename T> class DataColumnSparseReverseIterator;
		}
	}

	namespace collections {
		template<>
		class DataContainer < daw::data::DataCell, ::std::map<size_t, daw::data::DataCell> > {
		public:
			using value_type = daw::data::DataCell;
			using reference = daw::data::DataCell&;
			using const_reference = const daw::data::DataCell&;
			using values_type = ::std::map < size_t, daw::data::DataCell > ;

			using iterator = daw::data::impl::DataColumnSparseIterator < daw::data::DataColumnSparse > ;
			using const_iterator = daw::data::impl::DataColumnSparseIterator < daw::data::DataColumnSparse const > ;
			using reverse_iterator = daw::data::impl::DataColumnSparseReverseIterator < daw::data::DataColumnSparse > ;
			using const_reverse_iterator = daw::data::impl::DataColumnSparseReverseIterator < daw::data::DataColumnSparse const > ;
			using difference_type = values_type::difference_type;
			using size_type = values_type::size_type;

			virtual iterator begin( ) = 0;
			virtual iterator end( ) = 0;
			virtual const_iterator begin( ) const = 0;
			virtual const_iterator end( ) const = 0;
			virtual const_iterator cbegin( ) const = 0;
			virtual const_iterator cend( ) const = 0;
			virtual reverse_iterator rbegin( ) = 0;
			virtual reverse_iterator rend( ) = 0;
			virtual const_reverse_iterator rbegin( ) const = 0;
			virtual const_reverse_iterator rend( ) const = 0;
			virtual const_reverse_iterator crbegin( ) const = 0;
			virtual const_reverse_iterator crend( ) const = 0;

			virtual size_type size( ) const = 0;
			virtual bool empty( ) const = 0;
			virtual void clear( ) = 0;

			virtual reference item( const size_type pos ) = 0;
			virtual const_reference item( const size_type pos ) const = 0;

			virtual reference operator[]( const size_type pos ) {
				return item( pos );
			}

			virtual const_reference operator[]( const size_type pos ) const {
				return item( pos );
			}

			virtual void append( const_reference ) = 0;
			virtual void append( value_type&& ) = 0;
		};
	}	// namespace collections
}	// namespace daw
