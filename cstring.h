#pragma once

#include <string>
#include "header_libraries/scope_guard.h"
#include "header_libraries/workarounds.h"
#include "header_libraries/daw_traits.h"

namespace daw {
	namespace data {
		template<typename CharType = char>
		class CString {
		public:
			CString( const CharType* c_str, bool do_copy = false, const size_t length = 0 ) :m_cstr( c_str ), m_local_string( do_copy ) {
				if( do_copy ) {
					size_t len = length;
					if( 0 == len ) {
						len = strlen( c_str );
					}
					CharType* tmp = nullptr;
					SCOPE_EXIT {
						m_cstr = tmp;
					};
					tmp = new_array_throw<CharType>( len + 1 );
					memcpy( tmp, c_str, len + 1 );
				}
			}

			CString( ) noexcept:m_cstr( nullptr ), m_local_string( true ) { }
			CString( const CString& value ) = delete;
			CString& operator=(const CString& rhs) = delete;

			CString( CString&& value ) noexcept: m_cstr( std::move( value.m_cstr ) ), m_local_string( std::move( value.m_local_string ) ) {
				value.nullify( );
			}

			CString& operator=(CString&& rhs) {
				if( this != &rhs ) {
					m_cstr = std::move( rhs.m_cstr );
					m_local_string = std::move( rhs.m_local_string );
					rhs.nullify( );
				}
				return *this;
			}

			~CString( ) noexcept {
				if( m_local_string && nullptr != m_cstr ) {
					delete[] m_cstr;
					nullify( );
				}
			}

			bool operator==(const CString& rhs) const = delete;

			const CharType& operator[]( size_t pos ) const {
				return m_cstr[pos];
			}

			void nullify( ) noexcept {
				m_cstr = nullptr;
				m_local_string = false;
			}

				const char* move( ) noexcept {
				auto result = m_cstr;
				nullify( );
				return result;
			}

				const char* get( ) noexcept {
				return m_cstr;
			}

				std::string to_string( ) const {
				return std::string( m_cstr );
			}

			size_t size( ) const noexcept {
				if( is_null( ) ) {
					return 0;
				}
				return strlen( m_cstr );
			}

				bool is_null( ) const noexcept {
				return nullptr == m_cstr;
			}

				bool is_local_string( ) const noexcept {
				return m_local_string;
			}

				void take_ownership_of_data( ) noexcept {
				m_local_string = true;
			}

		private:
			const CharType* m_cstr;
			bool m_local_string;
		};

		static_assert(daw::traits::is_regular<CString<char>>::value, "CString isn't regular");

		using cstring = CString < char > ;
	}	// namespace data
}	// namespace daw
