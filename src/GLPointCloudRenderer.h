#ifndef __GL_POINT_CLOUD_RENDERER__
#define __GL_POINT_CLOUD_RENDERER__

// stl include
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// opencv
#include <opencv2/opencv.hpp>

// local
#include "VertexBuffers.h"			// create vertex buffer object
#include "ShaderProgram.h"			// create a shader program
#include "Types.h"

using namespace std;


class GLPointCloudRenderer{

public:

        /*
		Create an point cloud model from a point cloud dataset
		@param pc - the point cloud dataset 
		*/
		GLPointCloudRenderer(vector<Eigen::Vector3f>& dst_points, vector<Eigen::Vector3f>& dst_normals);



		/*
		Update the points using the existing references. 
		*/
		void updatePoints(void);


		/*
		Draw the obj model
		@param viewMatrix - a view matrix object
		@param projectionMatrix - a  4x4 projection matrix .
		*/
		void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);


		/*
		Return the shader program
		@return - int containing the shader program
		*/
		int getProgram(void){return program;}


        /*
        Set the model matrix for this object.
        @param m - glm 4x4 matrix 
        */
        void setModelmatrix(glm::mat4 m){modelMatrix = m;}

private:

		int vaoID[1]; // Our Vertex Array Object
		int vboID[2]; // Our Vertex Buffer Object
		int iboID[1]; // Our Index  Object
		int program;
		int program_backup;

		int viewMatrixLocation;
		int modelMatrixLocation;
		int projMatrixLocation;


        std::vector<glm::vec3> points; 
	    std::vector<glm::vec3> normals;

		int _pos_location;
		int _norm_location;
		int _block;

        glm::mat4  modelMatrix; 
		glm::mat4	_projectionMatrix;
		glm::mat4	_viewMatrix;

	
		int _N; // number of vertices
		int _I; // number indices


		vector<Eigen::Vector3f>& _points;
		vector<Eigen::Vector3f>& _normals;


};



#endif