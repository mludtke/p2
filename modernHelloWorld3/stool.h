/*	Perry Kivolowitz - University of Wisconsin - Madison 
	Computer Sciences Department

	A sample hello world like program demonstrating modern
	OpenGL techniques. 

	Created:	2/25/13
	Updates:
*/

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "object.h"
#include "shader.h"

class stool : public Object
{
public:
	stool();
	bool Initialize(int slices, int stacks, int shader);
	void legs(glm::mat4 m, int stacks);
	virtual void Draw(const glm::ivec2 & size);
	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size, GLfloat xRot, GLfloat yRot, const float time = 0);
	void TakeDown();
	Shader shader;
	Shader solid_color;

	//bool Initialize_sphere(int slices, int stacks);
	//virtual void Draw_sphere(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0);

private:
	void BuildNormalVisualizationGeometry(int k);
	void BuildNormalVisualizationGeometry2();
	glm::vec4 colors[2];
	std::vector<VertexAttributesPCN> vertices;
	typedef Object super;
};
