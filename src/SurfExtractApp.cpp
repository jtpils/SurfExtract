#include "SurfExtractApp.h"



SurfExtractApp::SurfExtractApp()
{
	_image_width = 1024; 
	_image_height = 1024;
	_window_width = 1024 * 2; 
	_window_height = 768 * 2;
	_pview_complete = false;
	_pview_start = false;
	_verbose = false;
	_enable_render_3d = true;
	_enable_render_points = true;
	_pview_camera_distance = 3.5;
	_pview_sub = 1;

	_pca = NULL;
	_gl_pc = NULL;
	_model = NULL;
	_output_path = "";
	_output_done = false;

	_renderer = new GLRenderer();
	_renderer->create(_window_width, _window_height, "Surface Extration");
	_renderer->addRenderFcn(std::bind(&SurfExtractApp::render_fcn, this, _1, _2));
	_renderer->addKeyboardCallback(std::bind(&SurfExtractApp::keyboard_cb, this, _1, _2));

	_pview = new PolyhedronViewRenderer(_window_width, _window_height, 1024, 1024);
	_pview->setVerbose(false); // set first to get all the output info
	_pview->setOutputPath("temp");

	_pca = new PointCloudAssembly();
	_pca->setPointCloudDensity(0.025);

	// point cloud renderer
	_gl_pc = new GLPointCloudRenderer(_pca->getPointCloud().points, _pca->getPointCloud().points);

	

	
}


SurfExtractApp::~SurfExtractApp()
{
	delete _gl_pc;
	delete _pca;
	delete _pview;
	if (_model) delete _model;
	delete _renderer;
}


/*
Load the 3D model from a file
*/
bool SurfExtractApp::loadModel(string path_to_file)
{
	// load the 3d model
	unsigned int program = cs557::LoadAndCreateShaderProgram("lit_scene.vs", "lit_scene.fs");
	_model = new cs557::OBJModel();
	_model->create(path_to_file, program);
	_model->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

	_light0.pos = glm::vec3(0.0f, 5.0f, 3.0f);
	_light0.dir = glm::vec3(0.0f, 0.0f, 0.0f);
	_light0.k1 = 0.1;
	_light0.intensity = 1.7;
	_light0.index = 0;
	_light0.apply(_model->getProgram());

	_light1.pos = glm::vec3(0.0f, 5.0f, -3.0f);
	_light1.dir = glm::vec3(0.0f, 0.0f, 0.0f);
	_light1.k1 = 0.1;
	_light1.intensity = 1.0;
	_light1.index = 1;
	_light1.apply(_model->getProgram());

	// set model to be rendered
	_pview->setModelFromFile(path_to_file);

	return true;
}


void SurfExtractApp::render_fcn(glm::mat4 proj_matrix, glm::mat4 view_matrix)
{

	if (!_pview_complete && _pview_start) {
		_pview_complete = _pview->draw_sequence();
		_pca->addData(_pview->getCurrData());

		_gl_pc->updatePoints();
	}
	else {
		if (!_output_done && _output_path.size() > 0) {
			_pca->writeToFileOBJ(_output_path);
			_output_done = true;
		}
	}
	


	if (_model && _enable_render_3d)
		_model->draw(proj_matrix, view_matrix);


	if(_gl_pc && _enable_render_points)
		_gl_pc->draw(proj_matrix, view_matrix);
}

/*
Set the prerenderer camera distanct
@param distance - the distance as float
*/
void SurfExtractApp::setCameraDistance(float distance)
{
	if (distance < 0.1) return ;

	_pview_camera_distance = distance;

	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0, _pview_camera_distance), glm::vec3(0.0f, 0.0f, 00.f), glm::vec3(0.0f, 1.0f, 0.0f));
	if (_renderer)
		_renderer->setViewMatrix(viewMatrix);
}


/*
Start the point cloud generation
*/
bool SurfExtractApp::start(void)
{
	_pview->create(_pview_camera_distance, _pview_sub);
	_pview_start = true;

	show3DModel(false);
	_renderer->start();

	return _pview_start;
}


/*
Return whether the renderer is done 
drawing all images for the surface sequence. 
@return false - if not done, otherwise true
*/
bool SurfExtractApp::done(void)
{
	return _pview_complete;
}


/*
Set the density of the point cloud
@param density - the point cloud density as voxel cell distance.
Must be >= 0.005
*/
void SurfExtractApp::setPointCloudDensity(float density)
{
	if (!_pca)return;

	_pca->setPointCloudDensity(density);
	
}

/*
Resample an existing point cloud
@param density - the point cloud density as voxel cell distance.
Must be >= 0.005
*/
void SurfExtractApp::resample(float density)
{
	if (!_pca)return;

	_pca->resample(density);
	_gl_pc->updatePoints();

	
	cout << "[INFO] - Sampled " << _pca->getNumPoints() << " points." << endl;
	
}


/*
Save the point cloud to an  obj file.
@param path - string containing the path and file. 
Ending must be obj.
*/
bool SurfExtractApp::saveToObj(string path)
{
	if (!_pca)return false;

	_output_path = path;
	return _pca->writeToFileOBJ(path);
}

bool SurfExtractApp::setVerbose(bool verbose){
	if (!_pca)return false;

	_verbose = verbose;
	_pca->setVerbose(verbose);
	return true;
}


/*
Save the point cloud to an  obj file once the procedure is complete
@param path - string containing the path and file. 
Ending must be obj.
*/
void SurfExtractApp::setOutputFilename(string path)
{
	_output_path = path;
}


/*
Keyboard callback for the renderer
*/
void SurfExtractApp::keyboard_cb(int key, int action)
{
	//cout << key << " : " << action << endl;

	switch (action) {
	case GLFW_RELEASE:

		switch (key) {
		case 44:  // ,
			_pview_camera_distance -= 0.1;
			setCameraDistance(_pview_camera_distance);
			break;
		case 46: // .
			_pview_camera_distance += 0.1;
			setCameraDistance(_pview_camera_distance);
			break;
		case 49: // 1
			if (_enable_render_3d)
				this->show3DModel(false);
			else this->show3DModel(true);
			break;
		case 50: // 2
			if (_enable_render_points)
				this->showPointCloud(false);
			else this->showPointCloud(true);				
			break;
		case 82: //r
			cout << "[Input] - Enter density (0.005 < x < 1.0), current: " << _pca->getPointCloudDensity() << " (press enter) \n";
			float density;
			std::cin >> density;
			resample(density);
			break;
		case 83: // s
			_pca->writeToFileOBJ(_output_path);
			break;

		}

		break;

	case GLFW_PRESS:
		break;
	}


}