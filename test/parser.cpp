/* Copyright (C) 2021 Luca Nigro and Matteo Zeccoli Marazzini

This file is part of image-renderer.

image-renderer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

image-renderer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with image-renderer.  If not, see <https://www.gnu.org/licenses/>. */

#include "parser.h"
#undef NDEBUG
#include <cassert>

void testSceneFile() {
	std::stringstream sstream;
	sstream << "abc   \nd\nef";
	InputStream stream{sstream};
	
	assert(stream.location.line == 1);
	assert(stream.location.col == 1);

	assert(stream.readChar() == 'a');
	assert(stream.location.line == 1);
	assert(stream.location.col == 2);

	stream.unreadChar('A');
	assert(stream.location.line == 1);
	assert(stream.location.col == 1);

	assert(stream.readChar() == 'A');
	assert(stream.location.line == 1);
	assert(stream.location.col == 2);

	assert(stream.readChar() == 'b');
	assert(stream.location.line == 1);
	assert(stream.location.col == 3);

	assert(stream.readChar() == 'c');
	assert(stream.location.line == 1);
	assert(stream.location.col == 4);

	stream.skipWhitespacesAndComments();

	assert(stream.readChar() == 'd');
	assert(stream.location.line == 2);
	assert(stream.location.col == 2);

	assert(stream.readChar() == '\n');
	assert(stream.location.line == 3);
	assert(stream.location.col == 1);

	assert(stream.readChar() == 'e');
	assert(stream.location.line == 3);
	assert(stream.location.col == 2);

	assert(stream.readChar() == 'f');
	assert(stream.location.line == 3);
	assert(stream.location.col == 3);

	assert(stream.readChar() == EOF);

}

int main() {
	testSceneFile();
	return 0;
}
