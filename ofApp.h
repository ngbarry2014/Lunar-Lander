#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "ParticleEmitter.h"
#include "Particle.h"



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent2(ofDragInfo dragInfo);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void integrate();
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		void checkCollisions();
		void loadVbo();

		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);

		ofEasyCam cam;
		ofxAssimpModelLoader moon, lander;
		ofLight light;
		Box boundingBox, landerBounds;
		Box testBox;
		vector<Box> colBoxList;
		bool bLanderSelected = false;
		Octree octree;
		TreeNode selectedNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;

		ofxIntSlider numLevels;
		ofxPanel gui;

		//Barry Ng
		glm::vec3 vel;
		glm::vec3 acc;
		glm::vec3 header;
		glm::vec3 gravity;
		glm::vec3 foward;
		float tvel;
		float tacc;
		float aacc;
		float avel;
		float rotation;
		float fuel;
		bool turning;
		bool thrusting;
		bool otherdir;
		bool onboardcam;
		bool tracking;
		bool easy;
		bool aglon;
		bool engineon;
		bool winner;
		Ray agl;

		//Barry Ng
		ofSoundPlayer rocket;
		ofSoundPlayer bigEx;
		ofSoundPlayer winSound;

		//Barry Ng
		ofImage bg;
		ofLight shipLight;
		ofLight sunLight;
		ofLight winLight;
		ParticleEmitter emitter;
		ofTexture  particleTex;
		ofVbo vbo;
		ofShader shader;
		TurbulenceForce* turbForce;
		GravityForce* gravityForce;
		ImpulseRadialForce* radialForce;
		ImpulseRadialForce* radialForce2;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		bool bHide;
		bool pointSelected = false;
		bool bDisplayLeafNodes = false;
		bool bDisplayOctree = false;
		bool bDisplayBBoxes = false;
		
		bool bLanderLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;

		vector<Box> bboxList;

		const float selectionRange = 4.0;
};
