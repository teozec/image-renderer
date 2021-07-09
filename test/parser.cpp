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

using namespace std;

void testSceneFile() {
	std::stringstream sstream;
	sstream << "abc   \nd\nef";
	InputStream stream{sstream, std::string{}};
	
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

void testLexer() {
	std::stringstream sstream;
	sstream <<
		"# This is a comment\n"
		"# This is another comment\n"
		"new material sky_material(\n"
		"	diffuse(image(\"my file.pfm\")),\n"
		"	<5.0, 500.0, 300.0>\n"
		") # Comment at the end ot the line\n";
	InputStream stream{sstream, SourceLocation{"file", 1, 1}};

	Token token = stream.readToken();
	assert(token.type == TokenType::KEYWORD);
	assert(token.value.k == Keyword::NEW);

	token = stream.readToken();
	assert(token.type == TokenType::KEYWORD);
	assert(token.value.k == Keyword::MATERIAL);

	token = stream.readToken();
	assert(token.type == TokenType::IDENTIFIER);
	assert(token.value.s == "sky_material");

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == '(');

	token = stream.readToken();
	assert(token.type == TokenType::KEYWORD);
	assert(token.value.k == Keyword::DIFFUSE);

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == '(');

	token = stream.readToken();
	assert(token.type == TokenType::KEYWORD);
	assert(token.value.k == Keyword::IMAGE);

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == '(');

	token = stream.readToken();
	assert(token.type == TokenType::STRING);
	assert(token.value.s == "my file.pfm");

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == ')');

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == ')');

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == ',');

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == '<');

	token = stream.readToken();
	assert(token.type == TokenType::FLOAT);
	assert(abs(token.value.f - 5.f) < 1e-5f);

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == ',');

	token = stream.readToken();
	assert(token.type == TokenType::FLOAT);
	assert(abs(token.value.f - 500.f) < 1e-5f);

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == ',');

	token = stream.readToken();
	assert(token.type == TokenType::FLOAT);
	assert(abs(token.value.f - 300.f) < 1e-5f);

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == '>');

	token = stream.readToken();
	assert(token.type == TokenType::SYMBOL);
	assert(token.value.ch == ')');

	token = stream.readToken();
	assert(token.type == TokenType::STOP);
	assert(token.value.ch == ')');
}

int main() {
	testSceneFile();
	testLexer();
	return 0;
}
