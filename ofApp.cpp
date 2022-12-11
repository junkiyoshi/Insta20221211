#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size();
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	this->location = location_list[this->select_index] + distance / frame_span * param;

	this->log.push_front(this->location);
	while (this->log.size() > 60) { this->log.pop_back(); }
}

//--------------------------------------------------------------
glm::vec3 Actor::getLocation() {

	return this->location;
}

//--------------------------------------------------------------
deque<glm::vec3> Actor::getLog() {

	return this->log;
}

//--------------------------------------------------------------
void Actor::setColor(ofColor color) {

	this->color = color;
}

//--------------------------------------------------------------
ofColor Actor::getColor() {

	return this->color;
}


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openFrameworks");

	ofBackground(0);
	ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ADD);
	ofEnableDepthTest();

	ofIcoSpherePrimitive ico_sphere = ofIcoSpherePrimitive(290, 5);
	vector<ofMeshFace> triangles = ico_sphere.getMesh().getUniqueFaces();
	for (int i = 0; i < triangles.size(); i++) {

		ofPoint average = (triangles[i].getVertex(0) + triangles[i].getVertex(1) + triangles[i].getVertex(2)) / 3;
		this->location_list.push_back(average);
	}

	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= 8) {

				next_index.push_back(index);
			}
		}

		this->next_index_list.push_back(next_index);
	}

	vector<ofColor> color_palette;
	for (int i = 0; i < 5; i++) {

		ofColor color;
		color.setHsb(ofMap(i, 0, 5, 0, 255), 200, 200);
		color_palette.push_back(color);
	}


	for (int i = 0; i < 2300; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
		this->actor_list.back()->setColor(color_palette[(int)ofRandom(color_palette.size())]);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 5;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateY(ofGetFrameNum() * 0.333333333333333333333);
	ofRotateX(ofGetFrameNum() * 0.333333333333333333333);

	ofFill();
	ofSetColor(0);
	ofDrawSphere(287);

	ofSetLineWidth(2);
	ofNoFill();
	for (auto& actor : this->actor_list) {

		ofSetColor(actor->getColor());

		ofBeginShape();
		for (auto& l : actor->getLog()) {

			ofVertex(l);
		}
		ofEndShape();
	}

	this->cam.end();
}


//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}