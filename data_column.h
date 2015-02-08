#pragma once

#include "data_types.h"
#include "data_cell.h"
#include <functional>
#include <string>
#include"workarounds.h"

#if USE_PPL == 1
#include <ppl.h>
#else
#ifdef _WIN32
#include <omp.h>
#endif
#endif


namespace daw {
	namespace data {
		template<typename StorageType>
		class DataColumn {
		public:
			using value_type = typename StorageType::value_type;
			using reference = value_type&;
			using const_reference = const value_type&;
			using values_type = StorageType;
			using iterator = typename values_type::iterator;
			using const_iterator = typename values_type::const_iterator;
			using reverse_iterator = typename values_type::reverse_iterator;
			using const_reverse_iterator = typename values_type::const_reverse_iterator;
			using difference_type = typename values_type::difference_type;
			using size_type = typename values_type::size_type;
		protected:
			values_type m_items;
			std::string m_header;
			bool m_hidden;

			reference item( const size_type pos ) {
				return m_items[pos];
			}

			const_reference item( const size_type pos ) const {
				return m_items[pos];
			}

		public:
			DataColumn( const std::string& header = "" ) noexcept : m_items( ), m_header( header ), m_hidden( false ) { }
			DataColumn( const DataColumn& ) = default;
			~DataColumn( ) = default;
			DataColumn( DataColumn&& other ) noexcept: m_items( std::move( other.m_items ) ), m_header( std::move( other.m_header ) ), m_hidden( std::move( other.m_hidden ) ) { }
			bool operator==(const DataColumn&) const = delete;

			DataColumn& operator=(DataColumn rhs) noexcept {
				m_items = std::move( rhs.m_items );
				m_header = std::move( rhs.m_header );
				m_hidden = std::move( rhs.m_hidden );
				return *this;
			}

				void shrink_to_fit( ) {
				shrink_to_fit( m_items );
			}

			template<typename Container>
			inline static void shrink_to_fit( Container& values ) {
				values.shrink_to_fit( );
			}

			void append( value_type value ) {
				m_items.push_back( std::move( value ) );
			}

			iterator erase( iterator first ) {
				auto ret = m_items.erase( first );
				return ret;
			}

			iterator erase( iterator first, iterator last ) {
				auto ret = m_items.erase( first, last );
				return ret;
			}

			void erase_item( const size_type pos ) {
				m_items.erase( m_items.begin( ) + pos );
			}

			const std::string& header( ) const {
				return m_header;
			}

			std::string& header( ) noexcept {
				return m_header;
			}

				bool& hidden( ) noexcept {
				return m_hidden;
			}

				const bool& hidden( ) const noexcept {
				return m_hidden;
			}

				reference operator[]( const size_type pos ) {
				return item( pos );
			}

			const_reference operator[]( const size_type pos ) const {
				return item( pos );
			}

			iterator begin( ) {
				return m_items.begin( );
			}

			iterator end( ) {
				return m_items.end( );
			}

			const_iterator begin( ) const {
				return m_items.begin( );
			}

			const_iterator end( ) const {
				return m_items.end( );
			}

			const_iterator cbegin( ) const {
				return m_items.cbegin( );
			}

			const_iterator cend( ) const {
				return m_items.cend( );
			}

			reverse_iterator rbegin( ) {
				return m_items.rbegin( );
			}

			reverse_iterator rend( ) {
				return m_items.rend( );
			}

			const_reverse_iterator rbegin( ) const {
				return m_items.rbegin( );
			}

			const_reverse_iterator rend( ) const {
				return m_items.rend( );
			}

			const_reverse_iterator crbegin( ) const {
				return m_items.crbegin( );
			}

			const_reverse_iterator crend( ) const {
				return m_items.crend( );
			}

			size_type size( ) const noexcept {
				return m_items.size( );
			}

				bool empty( ) const noexcept {
				return m_items.empty( );
			}

				void clear( ) {
				m_items.clear( );
			}
		};
	}	// namespace data
}	// namespace daw
