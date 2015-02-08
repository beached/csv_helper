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
