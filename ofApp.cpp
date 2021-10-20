
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"
#include <glm/gtx/intersect.hpp>


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
//	ofSetWindowShape(1024, 768);
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();
	// setup rudimentary lighting 
	//
	ofBackground(0, 0, 0);
	bg.load("Images/space.jpg");

	moon.loadModel("geo/moon-houdini.obj");
	moon.setScaleNormalization(false);

	//particles
	ofDisableArbTex();
	ofLoadImage(particleTex, "images/dot.png");
	shader.load("shaders_gles/shader");
	shader.load("shaders/shader");



	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 10, 1, 20));
	bHide = false;

	//  Create Octree for testing.
	//
	
	float time = ofGetElapsedTimeMillis();
	octree.create(moon.getMesh(0), 20);
	cout << "Time taken to create tree: " << ofGetElapsedTimeMillis() - time << " milliseconds" << endl;
	cout << "Number of Verts: " << moon.getMesh(0).getNumVertices() << endl;


	gravity = glm::vec3(0, -1, 0);



	header = glm::vec3(1, 0, 0);

	testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));
	lander.loadModel("geo/lander.obj");
	bLanderLoaded = true;
	lander.setScaleNormalization(false);
	lander.setPosition(ofRandom(-250, 250), ofRandom(0, 50), ofRandom(-250, 250));
	bboxList.clear();
	for (int i = 0; i < lander.getMeshCount(); i++) {
		bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
	}


	//Barry Ng
	turbForce = new TurbulenceForce(ofVec3f(-10, -10, -10), ofVec3f(10, 10, 10));
	gravityForce = new GravityForce(ofVec3f(0, -50, 0));
	radialForce = new ImpulseRadialForce(200);
	emitter.sys->addForce(turbForce);
	emitter.sys->addForce(gravityForce);
	emitter.sys->addForce(radialForce);
	emitter.setVelocity(ofVec3f(0, 0, 0));
	emitter.setOneShot(true);
	emitter.setEmitterType(RadialEmitter);
	emitter.setGroupSize(100);
	emitter.setRandomLife(true);
	emitter.setLifespanRange(ofVec2f(0.1, 0.2));

	//Barry Ng
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	Vector3 rayo = bounds.center();
	Vector3 rayd = Vector3(0, -1, 0);
	agl = Ray(rayo, rayd);
	ofVec3f epos = lander.getPosition();
	epos.y = epos.y;
	emitter.setPosition(epos);

	//Barry Ng
	onboardcam = false;
	tracking = true;
	easy = false;
	cam.disableMouseInput();
	cam.setPosition(ofVec3f(-50, 40, -150));
	fuel = 10;
	bHide = true;

	//Barry Ng
	shipLight.setup();
	shipLight.enable();
	shipLight.setSpotlight();
	shipLight.setScale(5);
	shipLight.setSpotlightCutOff(100);
	shipLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	shipLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	shipLight.setSpecularColor(ofFloatColor(1, 1, 1));
	shipLight.rotate(-90, ofVec3f(1, 0, 0));
	shipLight.setPosition(lander.getPosition());
	
	//Barry Ng
	sunLight.setup();
	sunLight.enable();
	sunLight.setSpotlight();
	sunLight.setScale(10);
	sunLight.setSpotlightCutOff(100);
	sunLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	sunLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	sunLight.setSpecularColor(ofFloatColor(1, 1, 1));
	sunLight.rotate(-45, ofVec3f(1, 0, 0));
	sunLight.rotate(225, ofVec3f(0, 1, 0));
	sunLight.setPosition(ofVec3f(-350, 200, -350));

	//Barry Ng
	winLight.setup();
	winLight.enable();
	winLight.setSpotlight();
	winLight.setScale(5);
	winLight.setSpotlightCutOff(100);
	winLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	winLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	winLight.setSpecularColor(ofFloatColor(1, 1, 1));
	winLight.rotate(-90, ofVec3f(1, 0, 0));
	winLight.setPosition(ofVec3f(0, 32, 0));
	glEnable(GL_DEPTH_TEST);

	//Barry Ng
	bigEx.load("Sounds/bigex.wav");
	winSound.load("Sounds/winner.wav");
	rocket.load("Sounds/rocket.wav");
	bigEx.setMultiPlay(true);
	winSound.setMultiPlay(true);
	rocket.setLoop(true);
}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
//

//Barry Ng
void ofApp::update() {
	if (winner) {
		return;
	}
	integrate();
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	Vector3 rayo = bounds.center();
	Vector3 rayd = Vector3(0, -1, 0);
	agl = Ray(rayo, rayd);
	ofSeedRandom();

	ofVec3f epos = lander.getPosition();
	epos.y = epos.y;
	emitter.setPosition(epos);
	emitter.update();

	if (onboardcam) {
		cam.setPosition(lander.getPosition());
		ofVec3f campos = lander.getPosition();
		campos.y = campos.y - 2;
		cam.setTarget(campos);
	}
	if (tracking) {
		cam.setTarget(lander.getPosition());
	}

	if (engineon) {
		fuel -= 1 / ofGetFrameRate();
	}
	shipLight.setPosition(lander.getPosition());
}
//--------------------------------------------------------------
void ofApp::draw() {
	loadVbo();


	if (!bHide) gui.draw();
	bg.draw(0, 0);
	cam.begin();
	glDepthFunc(GL_ALWAYS);
	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		moon.drawWireframe();
		if (bLanderLoaded) {
			lander.drawWireframe();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		moon.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			lander.setRotation(0, rotation, 0, 1, 0);
			lander.drawFaces();
			ofPopMatrix();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
			if (bDisplayBBoxes) {
				ofNoFill();
				ofSetColor(ofColor::white);
				for (int i = 0; i < lander.getNumMeshes(); i++) {
					ofPushMatrix();
					ofMultMatrix(lander.getModelMatrix());
					ofRotate(-90, 1, 0, 0);
					Octree::drawBox(bboxList[i]);
					ofPopMatrix();
				}
			}

			if (bLanderSelected) {

				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofSetColor(ofColor::white);
				Octree::drawBox(bounds);

				// draw colliding boxes
				//
				/*
				ofSetColor(ofColor::lightBlue);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
				*/
			}
		}
	}
	for (int i = 0; i < colBoxList.size(); i++) {
		Octree::drawBox(colBoxList[i]);
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));



	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		moon.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);
	}




	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	//Barry Ng
	Vector3 c = selectedNode.box.center();
	float dist = lander.getPosition().y - c.y();
	if (aglon) {
		if (octree.intersect(agl, octree.root, selectedNode)) {
			float dist10 = dist / 15;
			for (int i = 1; i < 14; i++) {
				Vector3 p = agl.origin;
				ofVec3f np = ofVec3f(p.x(), (p.y() - (dist10 * i)), p.z());
				ofVec3f d = np - cam.getPosition();
				ofDrawSphere(np, .01 * d.length());
			}
			/*
			Vector3 c = selectedNode.box.center();
			ofVec3f p = ofVec3f(c.x(), c.y(), c.z());
			ofVec3f d = p - cam.getPosition();
			ofSetColor(ofColor::lightGreen);
			ofDrawSphere(p, .02 * d.length());
			*/
			Vector3 p = agl.origin;
			ofVec3f np = ofVec3f(p.x(), (p.y() - (dist)), p.z());
			ofVec3f d = np - cam.getPosition();
			ofSetColor(ofColor::lightGreen);
			ofDrawSphere(np, .02 * d.length());
		}
	}
	/*
	if (pointSelected) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - cam.getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}
	*/

	ofPopMatrix();
	glDepthFunc(GL_LESS);
	cam.end();
	glDepthMask(GL_FALSE);

	ofSetColor(255, 100, 90);

	// this makes everything look glowy :)
	//
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();


	// begin drawing in the camera
	//
	shader.begin();
	cam.begin();

	// draw particle emitter here..
	//
	//emitter.draw();
	particleTex.bind();
	vbo.draw(GL_POINTS, 0, (int)emitter.sys->particles.size());
	particleTex.unbind();

	//  end drawing in the camera
	// 
	cam.end();
	shader.end();

	ofDisablePointSprites();
	ofDisableBlendMode();
	ofEnableAlphaBlending();

	// set back the depth mask
	//
	glDepthMask(GL_TRUE);

	string str;
	str += "Distance: " + std::to_string(dist);
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);
	str = "Fuel: " + std::to_string(fuel);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 50);
}

void ofApp::loadVbo() {
	if (emitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < emitter.sys->particles.size(); i++) {
		points.push_back(emitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(10));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case '1':
		onboardcam = true;
		tracking = false;
		easy = false;
		cam.disableMouseInput();
		break;
	case '2':
		onboardcam = false;
		tracking = true;
		easy = false;
		cam.disableMouseInput();
		cam.setPosition(ofVec3f(-50, 40, -150));
		break;
	case '3':
		onboardcam = false;
		tracking = false;
		easy = true;
		break;
	case 'h':
		break;
	case 'q':
		if (aglon) {
			aglon = false;
		}
		else {
			aglon = true;
		}
		break;
	case 'L':
	case 'l':
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'r':
		cam.reset();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'a':
		turning = true;
		aacc = 10;
		break;
	case 'd':
		turning = true;
		aacc = -10;
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		if (easy) {
			cam.enableMouseInput();
			bAltKeyDown = true;
		}
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	//Barry Ng
	case ' ':
		if (fuel > 0) {
			acc = glm::vec3(0, 2, 0);
			engineon = true;
			emitter.sys->reset();
			emitter.start();
			if (!rocket.isPlaying()) {
				rocket.play();
			}
		}
		break;
	case OF_KEY_LEFT:
		if (fuel > 0) {
			if (otherdir || tvel == 0) {
				tacc = -1;
				thrusting = true;
				otherdir = true;
				engineon = true;
				emitter.sys->reset();
				emitter.start();
				if (!rocket.isPlaying()) {
					rocket.play();
				}
			}
		}
		break;
	case OF_KEY_RIGHT:
		if (fuel > 0) {
			if (otherdir || tvel == 0) {
				tacc = 1;
				thrusting = true;
				otherdir = true;
				engineon = true;
				emitter.sys->reset();
				emitter.start();
				if (!rocket.isPlaying()) {
					rocket.play();
				}
			}
		}
		break;
	case OF_KEY_UP:
		if (fuel > 0) {
			if (!otherdir || tvel == 0) {
				tacc = -1;
				thrusting = true;
				otherdir = false;
				engineon = true;
				emitter.sys->reset();
				emitter.start();
				if (!rocket.isPlaying()) {
					rocket.play();
				}
			}
		}
		break;
	case OF_KEY_DOWN:
		if (fuel > 0) {
			if (!otherdir || tvel == 0) {
				tacc = 1;
				thrusting = true;
				otherdir = false;
				engineon = true;
				emitter.sys->reset();
				emitter.start();
				if (!rocket.isPlaying()) {
					rocket.play();
				}
			}
		}
		break;
	default:
		break;
	}
}

//Barry Ng
void ofApp::checkCollisions() {
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	colBoxList.clear();
	octree.intersect(bounds, octree.root, colBoxList);
}

//Barry Ng
void ofApp::integrate() {
	if (!turning && avel != 0) {
		if ((avel + aacc > 0 && avel < 0) || (avel + aacc < 0 && avel > 0)) {
			avel = 0;
			aacc = 0;
		}
		else {
			if (avel > 0) {
				aacc = -5;
			}
			else {
				aacc = 5;
			}
		}
	}

	avel += aacc / 60;
	rotation += avel / 60;

	float angle = rotation * PI / 180;

	//glm::vec3 tempvel = glm::vec3(1, 0, 0);
	//tempvel = glm::rotateY(tempvel, angle);
	//glm::vec3 noy = glm::vec3(vel.x, 0, vel.z);
	//float length = glm::length(noy);
	//float yvel = vel.y;
	//vel = tempvel * length;
	//vel.y = yvel;

	if (!thrusting && tvel != 0) {
		if ((tvel + tacc > 0 && tvel < 0) || (tvel + tacc < 0 && tvel > 0)) {
			tvel = 0;
			tacc = 0;
		}
		else {
			if (tvel > 0) {
				tacc = -0.5;
			}
			else {
				tacc = 0.5;
			}
		}
	}

	tvel += tacc / 60;
	glm::vec3 vvel;
	if (otherdir) {
		vvel = glm::vec3(tvel, 0, 0);
	}
	else {
		vvel = glm::vec3(0, 0, tvel);
	}
	vvel = rotateY(vvel, angle);
	vel = glm::vec3(vvel.x, vel.y, vvel.z);

	vel += acc / 60;
	vel += gravity / 60;
	vel += glm::vec3(ofRandom(-1, 1), 0, ofRandom(-1, 1));
	glm::vec3 pos = lander.getPosition();
	pos += vel / 60;

	glm::vec3 oldpos = lander.getPosition();
	lander.setPosition(pos.x, pos.y, pos.z);
	checkCollisions();
	if (colBoxList.size() != 0) {
		if (glm::length(vel) > 10) {
			emitter.setEmitterType(RadialEmitter);
			emitter.setGroupSize(10000);
			radialForce->set(10000);
			radialForce->setHeight(2);
			gravityForce->set(ofVec3f(0, -1, 0));
			emitter.lifeMinMax = ofVec2f(1, 2);
			emitter.sys->reset();
			emitter.start();
			vel = glm::vec3(ofRandom(-1000, 1000), ofRandom(0, 100), ofRandom(-1000, 1000));
			shipLight.disable();
			bigEx.play();
		}
		else if (tvel == 0) {
			if (oldpos.x > -24 && oldpos.x < 24 && oldpos.z > -24 && oldpos.z < 24) {
				Vector3 c = selectedNode.box.center();
				float dist = lander.getPosition().y - c.y();
				if (dist < 2 && dist > -1) {
					winner = true;
					winSound.play();
				}
			}
		}
		float epsilon = 1.5;
		glm::vec3 negativevel = -vel;
		negativevel *= epsilon;
		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();
		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		Vector3 c = bounds.center();
		Vector3 cc = colBoxList[0].center();
		glm::vec3 ccc = glm::vec3(c.x(), c.y(), c.z());
		glm::vec3 cccc = glm::vec3(cc.x(), cc.y(), cc.z());
		glm::vec3 norm = ccc - cccc;
		norm = glm::normalize(norm);
		float dot = glm::dot(norm, negativevel);
		norm = norm * dot;
		if (norm.y < 0) {
			norm.y = -norm.y;
		}
		vel = norm * 0.8;
		oldpos += vel / 60;
		lander.setPosition(oldpos.x, oldpos.y, oldpos.z);
	}


}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case 'a':
		turning = false;
		aacc = 0;
		break;
	case 'd':
		turning = false;
		aacc = 0;
		break;
	case ' ':
		acc = glm::vec3(0, 0, 0);
		engineon = false;
		rocket.stop();
		break;
	case OF_KEY_LEFT:
		thrusting = false;
		engineon = false;
		tacc = 0;
		rocket.stop();
		break;
	case OF_KEY_RIGHT:
		thrusting = false;
		engineon = false;
		tacc = 0;
		rocket.stop();
		break;
	case OF_KEY_UP:
		thrusting = false;
		engineon = false;
		tacc = 0;
		rocket.stop();
		break;
	case OF_KEY_DOWN:
		thrusting = false;
		engineon = false;
		tacc = 0;
		rocket.stop();
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			//anderSelected = true;
			//useDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
			//useLastPos = mouseDownPos;
			//nDrag = true;
			if (easy) {
				tracking = true;
			}

		}
		//Barry Ng
		else {
			bLanderSelected = false;
			if (easy) {
				ofVec3f p;
				raySelectWithOctree(p);
				tracking = false;
				cam.setTarget(ofVec3f(p.x, p.y, p.z));
			}
		}
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	float time = ofGetElapsedTimeMillis();
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	pointSelected = octree.intersect(ray, octree.root, selectedNode);

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	cout << "Time taken to find ray intersection: " << ofGetElapsedTimeMillis() - time << " milliseconds" << endl;
	return pointSelected;
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
	

		/*if (bounds.overlap(testBox)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "OK" << endl;
		}*/


	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
	if (lander.loadModel(dragInfo.files[0])) {
		lander.setScaleNormalization(false);
//		lander.setScale(.1, .1, .1);
	//	lander.setPosition(point.x, point.y, point.z);
		lander.setPosition(1, 1, 0);

		bLanderLoaded = true;
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		cout << "Mesh Count: " << lander.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (lander.loadModel(dragInfo.files[0])) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		//		lander.setRotation(1, 180, 1, 0, 0);

				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = cam.getPosition();
		glm::vec3 camAxis = cam.getZAxis();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = lander.getSceneMin();
			glm::vec3 max = lander.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for lander while we are at it
			//
			landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
	}


}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}
