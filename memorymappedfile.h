// MemoryMappedFile_private.h
#pragma once

#include <boost/iostreams/positioning.hpp>
#include <memory>
#include <string>
#include "workarounds.h"

namespace daw {
	namespace filesystem {
		class MemoryMappedFilePIMPL;

		class MemoryMappedFile {
		public:			
			MemoryMappedFile( MemoryMappedFile&& ) noexcept;
			MemoryMappedFile( const std::string &filename, const bool readonly = true );			
			MemoryMappedFile& operator=(MemoryMappedFile&& rhs) noexcept;
			
			char* data( const boost::iostreams::stream_offset position = 0 ) const;
			const char& operator[]( const boost::iostreams::stream_offset position ) const;
			char& operator[]( const boost::iostreams::stream_offset position );

			boost::iostreams::stream_offset size( ) const;
			bool is_open( ) const;
			void close( );
			~MemoryMappedFile( );

			MemoryMappedFile( ) = delete;
			MemoryMappedFile( const MemoryMappedFile& ) = delete;
			MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
		private:
			std::unique_ptr<MemoryMappedFilePIMPL> m_impl;
		};
	}	// namespace filesystem
}	// namespace daw

