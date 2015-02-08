#pragma once

#include "defs.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <vector>

#if USE_PPL == 1
#include <ppl.h>
#else
#ifdef _WIN32
#include <omp.h>
#endif
#endif

namespace daw {
	namespace data {
		namespace algorithm {

			template<typename ContainerType>
			void parallel_for_each( ContainerType& container, const std::function<void( typename ContainerType::reference )>& func ) {
#if USE_PPL == 1
				Concurrency::parallel_for_each( std::begin( container ), std::end( container ), func );
#else
#pragma omp parallel for
				for( ptrdiff_t n = 0; n < static_cast<ptrdiff_t>(container.size( )); ++n ) {
					func( container.item(n) );
				}
#endif
			}

			template<typename ContainerType>
			void parallel_for_each( const ContainerType& container, const std::function<void( typename ContainerType::const_reference )>& func ) {
#if USE_PPL == 1
				Concurrency::parallel_for_each( begin( container ), end( container ), func );
#else
#pragma omp parallel for
				for( ptrdiff_t n = 0; n < static_cast<ptrdiff_t>(container.size( )); ++n ) {
					func( container.item(n) );
				}
#endif
			}

			template<typename ContainerType>
			void for_each( ContainerType& container, const std::function<void( typename ContainerType::reference )>& func ) {
				for( auto& item : container ) {
					func( item );
				}
			}

			template<typename ContainerType>
			void for_each( const ContainerType& container, const std::function<void( typename ContainerType::const_reference )>& func ) {
				for( auto& item : container ) {
					func( item );
				}
			}

			template<typename ContainerType>
			void erase( ContainerType& container, const size_t pos ) {
				container.erase_item( pos );
			}

			template<typename ContainerType>
			void erase( const ContainerType& container, const size_t pos ) {
				static_assert(std::is_const<ContainerType>::value, "Must pass a non-constant container to erase_items");
			}

			template<typename ContainerType>
			void erase_items( ContainerType& container, const std::function<bool( typename ContainerType::const_reference )>& func ) {
				static_assert(!std::is_const<ContainerType>::value, "Not implemented");
// 				container.erase( std::remove_if( std::begin( container ), std::end( container ), func ), std::end( container ) );
			}

			template<typename ContainerType>
			void erase_items( const ContainerType& container, const std::function<bool( typename ContainerType::const_reference )>& func ) {
				// Compile time error so this is not used
				static_assert(std::is_const<T>::value, "Must pass a non-constant container to erase_items");
			}

			template<typename ContainerType>
			void erase_items( ContainerType& container, std::vector<size_t> items ) {
				for( auto& item : items ) {
					erase( container, item );
				}
			}

			template<typename ContainerType>
			void erase_items( const ContainerType& container, std::vector<size_t> items ) {
				static_assert(std::is_const<ContainerType>::value, "Must pass a non-constant container to erase_items");
			}
		}	// namespace algorithm
	}	// namespace data
}	// namespace daw
