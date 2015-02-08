// MemoryMappedFile_private.h
#pragma once

#include <boost/iostreams/positioning.hpp>
#include <memory>
#include <string>
#include "header_libraries/workarounds.h"

namespace daw {
	namespace filesystem {
		class MemoryMappedFileImpl;

		class MemoryMappedFile {
		public:			
			MemoryMappedFile( MemoryMappedFile&& ) noexcept;
			MemoryMappedFile( const std::string &filename, const bool readonly = true );			
			MemoryMappedFile& operator=(MemoryMappedFile&& rhs) noexcept;
			
			char* data( size_t const & position = 0 ) const;
			const char& operator[]( size_t const & position ) const;
			char& operator[]( size_t const & position );

			size_t size( ) const;
			bool is_open( ) const;
			void close( );
			~MemoryMappedFile( );

			MemoryMappedFile( ) = delete;
			MemoryMappedFile( const MemoryMappedFile& ) = delete;
			MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
		private:
			std::unique_ptr<MemoryMappedFileImpl> m_impl;
		};
	}	// namespace filesystem
}	// namespace daw

