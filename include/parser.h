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

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <sstream>
#include <cctype>
#include <unordered_map>

#define WHITESPACE std::string{" #\t\n\r"}
#define SYMBOLS std::string{"()<>[],*"}

/**
 * @brief   Location of the token.
 * @details Filename is given as well as line number and column namber.
 */
struct SourceLocation {
	std::string filename;
	int line{1};
	int col{1};
};

/**
 * @brief An exception thrown when a grammar error in the input scenefiles
 */
struct GrammarError : std::runtime_error {
	SourceLocation location;
	GrammarError(SourceLocation location, std::string message) : location{location}, std::runtime_error(message) {}
};

/**
 * @brief An enum to identify the available keywords
 */
enum class Keyword {
	NEW, MATERIAL, PLANE, SPHERE, DIFFUSE, SPECULAR, UNIFORM, CHECKERED,
	IMAGE, IDENTITY, TRANSLATION, ROTATION_X, ROTATION_Y, ROTATION_Z,
	SCALING, CAMERA, ORTHOGONAL, PERSPECTIVE, FLOAT
};

/**
 * @brief An enum to identify the available tokens
 */
enum class TokenType {
	KEYWORD, IDENTIFIER, STRING, FLOAT, SYMBOL, STOP
};

/**
 * @brief A union to store the value of a token
 */
union TokenUnion {
	TokenUnion() {}
	~TokenUnion() {}
	Keyword k;
	std::string s;
	float f;
	char ch;
};

/**
 * @brief A struct to store tokens
 */
struct Token {
	SourceLocation location;
	TokenType type;
	TokenUnion value;

	Token(SourceLocation location) : location{location} {}
	Token(const Token &other) : location{other.location}, type{other.type} {
		switch (type) {
		case TokenType::KEYWORD:
			value.k = other.value.k;
			break;
		case TokenType::IDENTIFIER:
		case TokenType::STRING:
			value.s = other.value.s;
			break;
		case TokenType::FLOAT:
			value.f = other.value.f;
			break;
		case TokenType::SYMBOL:
			value.ch = other.value.ch;
			break;
		}
	}

	void assignKeyword(Keyword k) {
		type = TokenType::KEYWORD;
		value.k = k;
	}

	void assignIdentifier(std::string s) {
		type = TokenType::IDENTIFIER;
		value.s = s;
	}

	void assignString(std::string s) {
		type = TokenType::STRING;
		value.s = s;
	}

	void assignFloat(float f) {
		type = TokenType::FLOAT;
		value.f = f;
	}

	void assignSymbol(char c) {
		type = TokenType::SYMBOL;
		value.ch = c;
	}

	void assignStop() {
		type = TokenType::STOP;
	}
};

/**
 * @brief   Wrapper used to parse the scene file.
 * @details It keeps updated the line number and the column number.
 *          It lets look-ahead tokens and un-read characters.
 */
struct InputStream {
	std::istream &stream;
	SourceLocation location;
	SourceLocation savedLocation;
	int tabulations;

	InputStream(std::istream &stream, int tabulations=4) : stream{stream} {}
	InputStream(std::istream &stream, SourceLocation location, int tabulations=4) : 
		stream{stream}, location{location}, tabulations{tabulations} {}

	Token parseStringToken() {
		std::string s{};
		Token token{location};
		for (;;) {
			switch (int ch = readChar()) {
			case std::char_traits<int>::eof():
				throw GrammarError(location, std::string{"unterminated string"});
				break;
			case '"':
				goto loopEnd;
				break;
			default:
				s += ch;
				break;
			}
		}
	loopEnd:
		token.assignString(s);
		return token;
	}

	Token parseFloatToken(char firstChar) {
		std::string s{firstChar};
		Token token{location};
		float f;
		for (;;) {
			int ch = readChar();
			if (std::isdigit(ch) or std::string{".eE+-"}.find(ch) != std::string::npos) {
				s += ch;
			} else if (ch == std::char_traits<int>::eof()) {
				break;
			} else {
				unreadChar(ch);
				break;
			}
		}
		try {
			f = std::stof(s);
		} catch (std::exception e) {
			throw GrammarError(location, std::string{s} + "is an invalid floating point number");
		}
		token.assignFloat(f);
		return token;
	}

	Token parseKeywordOrIdentifierToken(char firstChar) {
		std::string s{firstChar};
		Token token{location};
		// Associate each keyword woth the Keyword enum value
		std::unordered_map<std::string, Keyword> const keywordTable = {
			{"new", Keyword::NEW}, {"material", Keyword::MATERIAL},
			{"plane", Keyword::PLANE}, {"sphere", Keyword::SPHERE},
			{"diffuse", Keyword::DIFFUSE}, {"specular", Keyword::SPECULAR},
			{"uniform", Keyword::UNIFORM}, {"checkered", Keyword::CHECKERED},
			{"image", Keyword::IMAGE}, {"identity", Keyword::IDENTITY},
			{"translation", Keyword::TRANSLATION}, {"rotation_x", Keyword::ROTATION_X},
			{"rotation_y", Keyword::ROTATION_Y}, {"rotation_z", Keyword::ROTATION_Z},
			{"scaling", Keyword::SCALING}, {"camera", Keyword::CAMERA},
			{"orthogonal", Keyword::ORTHOGONAL}, {"perspective", Keyword::PERSPECTIVE},
			{"float", Keyword::FLOAT}
		};
		for (;;) {
			int ch = readChar();
			if (std::isalnum(ch) or ch == '_') {
				s += ch;
			} else if (ch == std::char_traits<int>::eof()) {
				break;
			} else {
				unreadChar(ch);
				break;
			}
		}
		auto it = keywordTable.find(s);
		if (it != keywordTable.end())
			token.assignKeyword(it->second);
		else
			token.assignIdentifier(s);
		return token;
	}

	void updatePosition(char ch) {
		if (ch == '\n'){
			location.line += 1;
			location.col = 1;
		} else if (ch == '\t') {
			location.col += tabulations;
		} else 
			location.col += 1;
	}

	int readChar() {
		int ch;
		ch = stream.get();
		if (ch != std::char_traits<int>::eof()) {
			savedLocation = location;
			updatePosition(ch);
		}
		return ch;
	}

	void unreadChar(int ch) {
		stream.putback(ch);
		location = savedLocation;
	}

	void skipWhitespacesAndComments() {
		char ch = readChar();
		while (std::string{WHITESPACE}.find(ch) != -1){
			if (ch == '#'){
				while (readChar()!='\r' and readChar()!='\n')
					continue;
			} else
				ch = readChar();
		}
		unreadChar(ch);
	}

	Token readToken() {
		skipWhitespacesAndComments();
		int ch = readChar();
		if (ch == std::char_traits<int>::eof()) {	// End of input stream
			Token stop{location};
			stop.assignStop();
			return stop;
		} else if (SYMBOLS.find(ch) != std::string::npos) {	// Symbol
			Token symbol{location};
			symbol.assignSymbol(ch);
			return symbol;
		} else if (ch == '"') {		// String
			return parseStringToken();
		} else if (std::isdigit(ch) or std::string{"+-."}.find(ch) != std::string::npos) {	// Float
			return parseFloatToken(ch);
		} else if (std::isalpha(ch) or ch == '_') {	// Keyword or identifier
			return parseKeywordOrIdentifierToken(ch);
		} else {
			throw GrammarError(location, "Invalid character " + std::string{static_cast<char>(ch)});
		}
	}
};

#endif // PARSER_H
