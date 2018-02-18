#pragma once
 
 #include <GL/glew.h>
 #include <glm/glm.hpp>
 #include <vector>
 
 #include "Vertex.h"
 
 namespace ArrowsIoEngine {
 
 	enum class GlyphSortType {
 		NONE,
 		FRONT_TO_BACK,
 		BACK_TO_FRONT,
 		TEXTURE                  // most efficient way , helps to render all the same textures together
 	};
 
 	struct Glyph { 
 		// sort according to texture so that same texture sprites can be rendered together
 		// minimum texture switching
 		GLuint texture;                                 
 		float depth;										
 
 		Vertex topLeft;
 		Vertex bottomLeft;
 		Vertex topRight;
 		Vertex bottomRight;
 	};
 
 	class RenderBatch {
 	public:
 		RenderBatch(GLuint Offset, GLuint NumVertices, GLuint Texture) : offset(Offset),
 			numVertices(NumVertices), texture(Texture) {}
 
 		GLuint offset;
 		GLuint numVertices;
 		GLuint texture;
 	};
 
 	class SpriteBatch
 	{
 	public:
 		SpriteBatch();
 		~SpriteBatch();
 
 		void init();
 
 		void begin(GlyphSortType sortType = GlyphSortType::TEXTURE);
 		void end();
 
 		// add to the render batch
 		void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Color& color);                 
 
 		// render the batch on screen
 		void renderBatch();           
 	
 	private:
 		void createRenderBatches();
 		void createVertexArray();
 		void sortGlyphs();
 
 		static bool compareFrontToBack(Glyph* a, Glyph* b);
 		static bool compareBackToFront(Glyph* a, Glyph* b);
 		static bool compareTexture(Glyph* a, Glyph* b);
 
 		GLuint _vbo;
 		GLuint _vao;
 
 		GlyphSortType _sortType;
 
 		std::vector<Glyph*> _glyphs;
 		std::vector<RenderBatch> _renderBatches;
 	};
 }