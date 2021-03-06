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
#include <vector>
#include <unordered_map>
#include <tuple>
#include <cmath>

#include "color.h"
#include "geometry.h"
#include "material.h"
#include "camera.h"
#include "shape.h"

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
	operator std::string() {
		std::ostringstream ss;
		ss << filename << ":" << line << "," << col;
		return ss.str();
	}
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
	NEW, MATERIAL, PLANE, SPHERE, TRIANGLE, DIFFUSE, SPECULAR, DIELECTRIC, UNIFORM, CHECKERED,
	IMAGE, IDENTITY, TRANSLATION, ROTATION_X, ROTATION_Y, ROTATION_Z,
	SCALING, CAMERA, ORTHOGONAL, PERSPECTIVE, FLOAT, UNION, DIFFERENCE, INTERSECTION, BOX
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

	Token(SourceLocation location = SourceLocation{}) : location{location} {}
	Token(const Token &other) : location{other.location}, type{other.type} {
		switch (type) {
		case TokenType::KEYWORD:
			value.k = other.value.k;
			break;
		case TokenType::IDENTIFIER:
		case TokenType::STRING:
			// The string value.s is not initialized, therefore we cannot use the assignement operator.
			// We need to initialize it, calling the constructor.
			new(&value.s) std::string{other.value.s};
			break;
		case TokenType::FLOAT:
			value.f = other.value.f;
			break;
		case TokenType::SYMBOL:
			value.ch = other.value.ch;
			break;
		case TokenType::STOP:
			break;
		}
	}

	Token operator=(const Token &other) {
		location = other.location;
		type = other.type;
		switch (type) {
		case TokenType::KEYWORD:
			value.k = other.value.k;
			break;
		case TokenType::IDENTIFIER:
		case TokenType::STRING:
			new(&value.s) std::string{other.value.s};
			break;
		case TokenType::FLOAT:
			value.f = other.value.f;
			break;
		case TokenType::SYMBOL:
			value.ch = other.value.ch;
			break;
		case TokenType::STOP:
			break;
		}
		return *this;
	}

	void assignKeyword(Keyword k) {
		type = TokenType::KEYWORD;
		value.k = k;
	}

	void assignIdentifier(std::string s) {
		type = TokenType::IDENTIFIER;
		new(&value.s) std::string{s};
	}

	void assignString(std::string s) {
		type = TokenType::STRING;
		new(&value.s) std::string{s};
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

	void assignKeywordOrIdentifier(std::string s) {
		auto it = keywordTable.find(s);
		if (it != keywordTable.end())
			assignKeyword(it->second);
		else
			assignIdentifier(s);
	}

	operator std::string() const {
		switch (type) {
		case TokenType::KEYWORD: {
			auto it = keywordTable.begin();
			for (; it != keywordTable.end(); it++)
				if (it->second == value.k)
					break;
			return "Keyword{" + it->first + "}";
		}
		case TokenType::IDENTIFIER:
			return "Identifier{" + value.s + "}";
		case TokenType::STRING:
			return "String{\"" + value.s + "\"}";
		case TokenType::FLOAT: {
			std::ostringstream ss;
			ss << "Float{\"" << value.f << "\"}";
			return ss.str();
		}
		case TokenType::SYMBOL: {
			std::ostringstream ss;
			ss << "Symbol{" << value.ch << "}";
			return ss.str();
		}
		case TokenType::STOP:
			return "Stop{}";
		}
		return std::string{};
	}
	
private:
	// Associate each keyword with the Keyword enum value
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
		{"float", Keyword::FLOAT}, {"union", Keyword::UNION},
		{"difference", Keyword::DIFFERENCE}, {"intersection", Keyword::INTERSECTION},
		{"box", Keyword::BOX}, {"dielectric", Keyword::DIELECTRIC},
		{"triangle", Keyword::TRIANGLE}
	};
};

struct FloatVariable {
	float value;
	bool isOverriden;
	FloatVariable() {}
	FloatVariable(float value, bool isOverriden = false): value{value}, isOverriden{isOverriden} {}
};

struct Scene {
	std::unordered_map<std::string, Material> materials;
	World world;
	std::shared_ptr<Camera> camera = nullptr;
	std::unordered_map<std::string, FloatVariable> floatVariables;
	float aspectRatio;
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
	Token savedToken{};
	bool isSavedToken = false;
	int tabulations;

	InputStream(std::istream &stream, std::string filename, int tabulations=4) : stream{stream}, tabulations{tabulations} {
		location.filename = filename;
	}
	InputStream(std::istream &stream, SourceLocation location, int tabulations=4) : 
		stream{stream}, location{location}, tabulations{tabulations} {}

	Token parseStringToken() {
		std::string s{};
		Token token{location};
		for (;;) {
			switch (int ch = readChar()) {
			case std::char_traits<char>::eof():
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
			} else if (ch == std::char_traits<char>::eof()) {
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
		for (;;) {
			int ch = readChar();
			if (std::isalnum(ch) or ch == '_') {
				s += ch;
			} else if (ch == std::char_traits<char>::eof()) {
				break;
			} else {
				unreadChar(ch);
				break;
			}
		}
		token.assignKeywordOrIdentifier(s);
		return token;
	}

	void updatePosition(char ch) {
		if (ch == '\n') {
			location.line += 1;
			location.col = 1;
		} else if (ch == '\t') {
			location.col += tabulations;
		} else {
			location.col += 1;
		}
	}

	int readChar() {
		int ch;
		ch = stream.get();
		if (ch != std::char_traits<char>::eof()) {
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
		int ch = readChar();
		while (std::string{WHITESPACE}.find(ch) != std::string::npos and ch != std::char_traits<char>::eof()) {
			if (ch == '#') {
				do
					ch = readChar();
				while (ch != '\r' and ch != '\n' and ch != std::char_traits<char>::eof());
			} else {
				ch = readChar();
			}
		}
		unreadChar(ch);
	}

	void unreadToken(Token token) {
		savedToken = token;
		isSavedToken = true;
	}

	Token readToken() {
		if (isSavedToken) {
			isSavedToken = false;
			return savedToken;
		}
		skipWhitespacesAndComments();
		int ch = readChar();
		if (ch == std::char_traits<char>::eof()) {	// End of input stream
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

	void expectSymbol(char ch) {
		Token token{readToken()};
		if (token.type != TokenType::SYMBOL or token.value.ch != ch)
			throw GrammarError(token.location, "Got " + std::string{token} + " instead of " + ch);
	}

	Keyword expectKeywords(std::vector<Keyword> ks) {
		Token token{readToken()};
		if (token.type != TokenType::KEYWORD)
			throw GrammarError(token.location, "Expected keyword, got " + std::string{token});
		for (auto it = ks.begin(); it != ks.end(); it++)
			if (*it == token.value.k)
				return *it;
		throw GrammarError(token.location, "Got unexpected " + std::string{token});
	}

	float expectNumber(Scene scene){
		Token token = readToken();
		if (token.type == TokenType::FLOAT)
			return token.value.f;
		else if (token.type == TokenType::IDENTIFIER){
			std::string varName = token.value.s;
			if (scene.floatVariables.find(varName) == scene.floatVariables.end())
				throw GrammarError(location, "Unknow variable "+varName);
			return scene.floatVariables[varName].value;
		}
		throw GrammarError(token.location, "Got "+std::string(token)+ ", expected a float");
	}

	std::string expectString(){
		Token token = readToken();
		if (!(token.type==TokenType::STRING))
			throw GrammarError(token.location, "Got "+std::string(token)+ ", expected a string");
		return token.value.s;
	}

	std::string expectIdentifier(){
		Token token = readToken();
		if (!(token.type==TokenType::IDENTIFIER))
			throw GrammarError(token.location, "Got "+std::string(token)+ ", expected an identifier");
		return token.value.s;
	}

	Vec parseVec(Scene scene) {
		expectSymbol('[');
		float x{expectNumber(scene)};
		expectSymbol(',');
		float y{expectNumber(scene)};
		expectSymbol(',');
		float z{expectNumber(scene)};
		expectSymbol(']');

		return Vec{x, y, z};
	}

	Color parseColor(Scene scene) {
		expectSymbol('<');
		float r{expectNumber(scene)};
		expectSymbol(',');
		float g{expectNumber(scene)};
		expectSymbol(',');
		float b{expectNumber(scene)};
		expectSymbol('>');

		return Color{r, g, b};
	}

	std::shared_ptr<Pigment> parsePigment(Scene scene) {
		Keyword k{expectKeywords(std::vector{Keyword::UNIFORM, Keyword::CHECKERED, Keyword::IMAGE})};
		std::shared_ptr<Pigment> result;

		expectSymbol('(');
		switch (k) {
		case Keyword::UNIFORM: {
			Color c{parseColor(scene)};
			result = std::make_shared<UniformPigment>(UniformPigment{c});
			break;
		}
		case Keyword::CHECKERED: {
			Color c1{parseColor(scene)};
			expectSymbol(',');
			Color c2{parseColor(scene)};
			expectSymbol(',');
			int n{(int) expectNumber(scene)};
			result = std::make_shared<CheckeredPigment>(CheckeredPigment{c1, c2, n});
			break;
		}
		case Keyword::IMAGE: {
			std::string file{expectString()};
			HdrImage image{file};
			expectSymbol(',');
			Color c{parseColor(scene)};
			result = std::make_shared<ImagePigment>(ImagePigment{image * c});
			break;
		}
		default:
			break;
		}
		expectSymbol(')');
		return result;
	}

	// diffuse(pigment)
	// specular(pigment, roughness)
	// dielectric(pigment, roughness, refractionIndex)
	std::shared_ptr<BRDF> parseBRDF(Scene scene) {
		Keyword k{expectKeywords(std::vector{Keyword::DIFFUSE, Keyword::SPECULAR, Keyword::DIELECTRIC})};
		expectSymbol('(');
		std::shared_ptr<Pigment> pigment{parsePigment(scene)};
		std::shared_ptr<BRDF> brdf;

		switch (k) {
		case Keyword::DIFFUSE:
			brdf = std::make_shared<DiffusiveBRDF>(DiffusiveBRDF{pigment});
			break;
		case Keyword::SPECULAR: {
			expectSymbol(',');
			float roughness{expectNumber(scene)};
			brdf = std::make_shared<SpecularBRDF>(SpecularBRDF{roughness, pigment});
			break;
		}
		case Keyword::DIELECTRIC: {
			expectSymbol(',');
			float roughness{expectNumber(scene)};
			expectSymbol(',');
			float refraction{expectNumber(scene)};
			brdf = std::make_shared<DielectricBSDF>(DielectricBSDF{refraction, roughness, pigment});
		}
		default:
			break;
		}
		expectSymbol(')');
		return brdf;
	}

	std::tuple<std::string, Material> parseMaterial(Scene scene) {
		std::string name{expectIdentifier()};
		expectSymbol('(');
		std::shared_ptr<BRDF> brdf{parseBRDF(scene)};
		expectSymbol(',');
		std::shared_ptr<Pigment> emittedRadiance{parsePigment(scene)};
		expectSymbol(')');

		return std::tuple<std::string, Material>{name, Material{brdf, emittedRadiance}};
	}

	Transformation parseTransformation(Scene scene) {
		Transformation result{};

		for (;;) {
			Keyword k{expectKeywords(std::vector{
				Keyword::IDENTITY, Keyword::TRANSLATION,
				Keyword::ROTATION_X, Keyword::ROTATION_Y,
				Keyword::ROTATION_Z, Keyword::SCALING})};

			switch (k) {
			case Keyword::IDENTITY:
				break;	// Identity doesn't change the matrix value
			case Keyword::TRANSLATION: {
				expectSymbol('(');
				Vec v{parseVec(scene)};
				expectSymbol(')');
				result *= translation(v);
				break;
			}
			case Keyword::ROTATION_X: {
				expectSymbol('(');
				float theta{expectNumber(scene)};
				expectSymbol(')');
				result *= rotationX(theta * M_PI/180);
				break;
			}
			case Keyword::ROTATION_Y: {
				expectSymbol('(');
				float theta{expectNumber(scene)};
				expectSymbol(')');
				result *= rotationY(theta * M_PI/180);
				break;
			}
			case Keyword::ROTATION_Z: {
				expectSymbol('(');
				float theta{expectNumber(scene)};
				expectSymbol(')');
				result *= rotationZ(theta * M_PI/180);
				break;
			}
			case Keyword::SCALING: {
				expectSymbol('(');
				Vec v{parseVec(scene)};
				expectSymbol(')');
				result *= scaling(v.x, v.y, v.z);
				break;
			}
			default:
				break;
			}
			Token token{readToken()};
			if (token.type != TokenType::SYMBOL or token.value.ch != '*') {
				unreadToken(token);
				break;
			}
		}	

		return result;
	}

	std::shared_ptr<Camera> parseCamera(Scene scene) {
		expectSymbol('(');
		Keyword typeKeyw = expectKeywords(std::vector{Keyword::PERSPECTIVE, Keyword::ORTHOGONAL});
		expectSymbol(',');
		Transformation transformation = parseTransformation(scene);
		expectSymbol(',');
		float distance = expectNumber(scene);
		expectSymbol(')');

		std::shared_ptr<Camera> cam;
		if (typeKeyw == Keyword::PERSPECTIVE)
			cam = std::make_shared<PerspectiveCamera>(PerspectiveCamera{scene.aspectRatio, transformation, distance});
		else if (typeKeyw == Keyword::ORTHOGONAL)
			cam = std::make_shared<OrthogonalCamera>(OrthogonalCamera{scene.aspectRatio, transformation});
		return cam;
	}

	// plane(material, transformation)
	Plane parsePlane(Scene scene){
		expectSymbol('(');
		std::string materialName = expectIdentifier();
		if (scene.materials.find(materialName) == scene.materials.end())
			throw GrammarError(location, "Unknown material "+materialName);
		expectSymbol(',');
		Transformation transformation = parseTransformation(scene);
		expectSymbol(')');

		return Plane{transformation, scene.materials[materialName]};
	}

	// sphere(material, transformation)
	Sphere parseSphere(Scene scene){
		expectSymbol('(');
		std::string materialName = expectIdentifier();
		if (scene.materials.find(materialName) == scene.materials.end())
			throw GrammarError(location, "Unknown material "+materialName);
		expectSymbol(',');
		Transformation transformation = parseTransformation(scene);
		expectSymbol(')');

		return Sphere{transformation, scene.materials[materialName]};
	}

	// triangle(material, pointA, pointB, pointC, transformation)
	Triangle parseTriangle(Scene scene){
		expectSymbol('(');
		std::string materialName = expectIdentifier();
		if (scene.materials.find(materialName) == scene.materials.end())
			throw GrammarError(location, "Unknown material "+materialName);
		expectSymbol(',');
		Vec vecA{parseVec(scene)};
		Point pointA{vecA.x, vecA.y, vecA.z};
		expectSymbol(',');
		Vec vecB{parseVec(scene)};
		Point pointB{vecB.x, vecB.y, vecB.z};
		expectSymbol(',');
		Vec vecC{parseVec(scene)};
		Point pointC{vecC.x, vecC.y, vecC.z};
		expectSymbol(',');
		Transformation transformation = parseTransformation(scene);
		expectSymbol(')');

		return Triangle{pointA, pointB, pointC, transformation, scene.materials[materialName]};
	}


	// box(material, pointMin, pointMax, transformation)
	Box parseBox(Scene scene){
		expectSymbol('(');
		std::string materialName = expectIdentifier();
		if (scene.materials.find(materialName) == scene.materials.end())
			throw GrammarError(location, "Unknown material "+materialName);
		expectSymbol(',');
		Vec vecMin{parseVec(scene)};
		Point pointMin{vecMin.x, vecMin.y, vecMin.z};
		expectSymbol(',');
		Vec vecMax{parseVec(scene)};
		Point pointMax{vecMax.x, vecMax.y, vecMax.z};
		expectSymbol(',');
		Transformation transformation = parseTransformation(scene);
		expectSymbol(')');

		return Box{pointMin, pointMax, transformation, scene.materials[materialName]};
	}


	std::shared_ptr<Shape> parseShape(Scene scene) {
		Keyword typeKeyw = expectKeywords(std::vector{Keyword::SPHERE, Keyword::PLANE, Keyword::UNION, Keyword::DIFFERENCE, Keyword::INTERSECTION, Keyword::BOX, Keyword::TRIANGLE});
		switch (typeKeyw) {
		case Keyword::SPHERE:
			return std::make_shared<Sphere>(parseSphere(scene));
		case Keyword::PLANE:
			return std::make_shared<Plane>(parsePlane(scene));
		case Keyword::UNION:
			return std::make_shared<CSGUnion>(parseUnion(scene));
		case Keyword::DIFFERENCE:
			return std::make_shared<CSGDifference>(parseDifference(scene));
		case Keyword::INTERSECTION:
			return std::make_shared<CSGIntersection>(parseIntersection(scene));
		case Keyword::BOX:
			return std::make_shared<Box>(parseBox(scene));
		case Keyword::TRIANGLE:
			return std::make_shared<Triangle>(parseTriangle(scene));
		default:
			exit(1);	// We should never get here
		}
	}


	CSGUnion parseUnion(Scene scene) {
		expectSymbol('(');
		auto shape1{parseShape(scene)};
		expectSymbol(',');
		auto shape2{parseShape(scene)};
		expectSymbol(',');
		auto transformation(parseTransformation(scene));
		expectSymbol(')');

		return CSGUnion(shape1, shape2, transformation);
	}

	CSGDifference parseDifference(Scene scene) {
		expectSymbol('(');
		auto shape1{parseShape(scene)};
		expectSymbol(',');
		auto shape2{parseShape(scene)};
		expectSymbol(',');
		auto transformation(parseTransformation(scene));
		expectSymbol(')');

		return CSGDifference(shape1, shape2, transformation);
	}

	CSGIntersection parseIntersection(Scene scene) {
		expectSymbol('(');
		auto shape1{parseShape(scene)};
		expectSymbol(',');
		auto shape2{parseShape(scene)};
		expectSymbol(',');
		auto transformation(parseTransformation(scene));
		expectSymbol(')');

		return CSGIntersection(shape1, shape2, transformation);
	}

	Scene parseScene(std::unordered_map<std::string, float> variables, float aspectRatio) {
		Scene scene;
		scene.aspectRatio = aspectRatio;

		// Insert the variables overriden from the function argument into the scene (if there's any)
		if (variables.size() != 0) {
			for (auto it = variables.begin(); it != variables.end(); it++)
				scene.floatVariables.insert({it->first, FloatVariable{it->second, true}});
		}

		for (;;) {
			Token token{readToken()};

			// End of input stream
			if (token.type == TokenType::STOP)
				break;

			// We expect a keyword
			if (token.type != TokenType::KEYWORD)
				throw GrammarError{token.location, "Expected keyword, got " + std::string{token}};

			switch (token.value.k) {
			case Keyword::FLOAT: {
				std::string name{expectIdentifier()};
				SourceLocation loc{location};
				expectSymbol('(');
				float value{expectNumber(scene)};
				expectSymbol(')');
				auto v = scene.floatVariables.find(name);
				if (v == scene.floatVariables.end())	// Variable not defined yet: add it to the list
					scene.floatVariables[name] = value;
				else if (!v->second.isOverriden)	// Variable defined two times in the file: error
					throw GrammarError{loc, "variable " + name + " already defined"};
				// The other possibility is that the variable is defined both in the file and in the variables function parameter.
				// In that case, we do nothing: the stored value is the one in the parameter.
				break;
			}
			case Keyword::SPHERE:
				scene.world.add(parseSphere(scene));
				break;
			case Keyword::PLANE:
				scene.world.add(parsePlane(scene));
				break;
			case Keyword::TRIANGLE:
				scene.world.add(parseTriangle(scene));
				break;
			case Keyword::CAMERA:
				if (scene.camera != nullptr)
					throw GrammarError{location, "Camera already defined"};
				scene.camera = parseCamera(scene);
				break;
			case Keyword::MATERIAL: {
				auto material{parseMaterial(scene)};
				scene.materials.insert({std::get<0>(material), std::get<1>(material)});
				break;
			}
			case Keyword::UNION:
				scene.world.add(parseUnion(scene));
				break;
			case Keyword::DIFFERENCE:
				scene.world.add(parseDifference(scene));
				break;
			case Keyword::INTERSECTION:
				scene.world.add(parseIntersection(scene));
				break;
			case Keyword::BOX:
				scene.world.add(parseBox(scene));
				break;
			default:
				throw GrammarError{location, "Unexpected keyword " + std::string{token}};
				break;
			}
		}
		
		if (scene.camera == nullptr)
			throw GrammarError{location, "No camera defined"};
		return scene;
	}
};

#endif // PARSER_H
