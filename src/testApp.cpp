#include "testApp.h"

bool testApp::drawData =false;

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(false);
	viewPort.set(0,0,ofGetScreenWidth(),ofGetScreenHeight());
	mouse_offset.set(0,0);
	currentLayer.setDims(ofVec2f(ofGetScreenWidth() * 1, ofGetScreenWidth() * 1));
	
	sender.setup( HOST, PORT );
	
	ofxOscMessage m;
	m.setAddress("/init");
	sender.sendMessage(m);
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	isFixed = false;
	pauseFollow = false;
	isPreview = false;
	trans.set(0,0);
	direction.set(1,0);
	
	rotY = 0;
	rotZ =0;
	targetZ = 0;
	lagCount = 0;

	isMouseDown = false;
	isOptionKey = false;
	mouseMode = MODE_NONE;
	currentAction = ACTION_NONE;
	buttonMode = 0;
	
	
	
	blipPreset::thisSynthDef.loadDictionary();
	
	for(int i =0 ; i < 4; i++){
		vector<blipPreset> t;
		presets.push_back(t);
	}
	
	loadPresets();
	selectedPreset[0] = 0;
	selectedPreset[1] = 0;
	
	thisReader.setLayer(&currentLayer);
	thisReader.setOscSender(&sender);
	
	currentLayer.getSM()->beginTrack(ofVec2f(0,0));
	currentLayer.getSM()->calcTrack(ofVec2f(100,0),ofVec2f(100,0), 1);
	currentLayer.getSM()->endTrack();
	
	
	
}

void testApp::loadPresets(){
	
	ofxXmlSettings XML;
	
	if(XML.loadFile("presets.xml")){
		
		if(XML.pushTag("SOUND_CIRCUIT", 0)){
			
			if(XML.pushTag("BLIP_PRESETS", 0)){
				
				int numBlips = XML.getNumTags("BLIP");
				
				for(int i = 0; i < numBlips; i++){
					
					if(XML.pushTag("BLIP", i)){
						
						blipPreset t_blip[4];
						
						for(int j = 0; j < 4; j++){
							t_blip[j].setName(XML.getValue("NAME", ""));
							t_blip[j].setSynthDef(XML.getValue("SYNTH", ""));
							t_blip[j].setDrawType(XML.getValue("DRAW", ""));
							t_blip[j].setEnvType(e_envType(XML.getValue("ENV", ENV_ASR)));
						}
						
						if(XML.pushTag("LENGTH", 0)){
							
							int numAlt = XML.getNumTags("ALT");
							
							for(int j = 0; j < 4; j++){
								paramAttributes * p = t_blip[j].getLength();
								
								if(j > 0 && j <= numAlt){
									if(XML.pushTag("ALT", j-1)){
										loadParamAttribute(XML, p);
										XML.popTag();
									}
								}else{
									loadParamAttribute(XML, p);
								}
							}
					
							XML.popTag();
						}
						
						if(XML.pushTag("ATTACK", 0)){ //continue updating from here ... needs a more efficient way of coding
							
							int numAlt = XML.getNumTags("ALT");
							
							for(int j = 0; j < 4; j++){
								
								if(j > 0 && j <= numAlt)XML.pushTag("ALT", j-1);
								
								if(XML.pushTag("PROP", 0)){
									t_blip[j].setIsAttackProp(true);
									paramAttributes * p = t_blip[j].getAttackProp();
									loadParamAttribute(XML, p);
									XML.popTag();
								}else{
									paramAttributes * p = t_blip[j].getAttackSecs();
									loadParamAttribute(XML, p);
									t_blip[j].setIsAttackProp(false);
								}
								
								if(j > 0 && j <= numAlt)XML.popTag();
							}
							

							XML.popTag();
						}
						
						if(XML.pushTag("DECAY", 0)){
							
							int numAlt = XML.getNumTags("ALT");
							
							for(int j = 0; j < 4; j++){
								
								if(j > 0 && j <= numAlt)XML.pushTag("ALT", j-1);
								
								if(XML.pushTag("PROP", 0)){
									t_blip[j].setIsDecayProp(true);
									paramAttributes * p = t_blip[j].getDecayProp();
									loadParamAttribute(XML, p);
									XML.popTag();
								}else{
									paramAttributes * p = t_blip[j].getDecaySecs();
									loadParamAttribute(XML, p);
									t_blip[j].setIsDecayProp(false);
								}
								
								if(j > 0 && j <= numAlt)XML.popTag();
								
							}
							
							XML.popTag();
						}
						
						if(XML.pushTag("POST_DECAY", 0)){
							
							int numAlt = XML.getNumTags("ALT");
							
							for(int j = 0; j < 4; j++){
								
								if(j > 0 && j <= numAlt)XML.pushTag("ALT", j-1);
								
								if(XML.pushTag("PROP", 0)){
									t_blip[j].setIsPostDecayProp(true);
									paramAttributes * p = t_blip[j].getPostDecayProp();
									loadParamAttribute(XML, p);
									XML.popTag();
								}else{
									paramAttributes * p = t_blip[j].getPostDecaySecs();
									loadParamAttribute(XML, p);
									t_blip[j].setIsPostDecayProp(false);
								}
								
								if(j > 0 && j <= numAlt)XML.popTag();
							}
							
							XML.popTag();
						}
						
						
						int numSound = XML.getNumTags("SOUND");
						int numVisual = XML.getNumTags("VISUAL");
						
						for(int j = 0; j < numSound; j ++){
							if(XML.pushTag("SOUND", j)){
								
								for(int k = 0; k < 4; k++){
									
									paramAttributes * p = t_blip[k].getSoundParam(XML.getValue("NAME", "")); 
									
									int numAlt = XML.getNumTags("ALT");
									
									if(k > 0 && k <= numAlt){
										if(XML.pushTag("ALT", k-1)){
											loadParamAttribute(XML, p);
											XML.popTag();
										}
									}else{
										loadParamAttribute(XML, p);
									}
									
								}
								XML.popTag();

							}
							
						}
						
						for(int j = 0; j < numVisual; j ++){
							if(XML.pushTag("VISUAL", j)){
								
								for(int k = 0; k < 4; k++){
									paramAttributes * p = t_blip[k].getVisualParam(XML.getValue("NAME", "")); 
									
									int numAlt = XML.getNumTags("ALT");
									
									if(k > 0 && k <= numAlt){
										if(XML.pushTag("ALT", k-1)){
											loadParamAttribute(XML, p);
											XML.popTag();
										}
									}else{
										loadParamAttribute(XML, p);
									}

									
								}
								
								XML.popTag();
							}
							
						}
						
						
						XML.popTag(); // blip pop
						
						for(int k = 0; k < 4; k++)presets[k].push_back(t_blip[k]);

					}
					
				} //loop end
			
				
				XML.popTag(); //outer tags
			}
			XML.popTag();
		}
	
	}else{
	
		cout << "file not found \n";
	}
	
	
}

void testApp::loadParamAttribute(ofxXmlSettings XML, paramAttributes * p){
	
	if(XML.tagExists("SET_TYPE", 0))p->setType = e_setType(XML.getValue("SET_TYPE", PSET_FIXED));
	if(XML.tagExists("ABS_VAL", 0))p->abs_value = XML.getValue("ABS_VAL", 0.5f);
	if(XML.tagExists("MIN_VAL", 0))p->min_val = XML.getValue("MIN_VAL", 0.0f);
	if(XML.tagExists("MAX_VAL", 0))p->max_val = XML.getValue("MAX_VAL", 1.0f);
	if(XML.tagExists("SLAVE_TO",0))p->slaveTo = XML.getValue("SLAVE_TO", "");
	
}



//--------------------------------------------------------------
void testApp::update(){
	
	bool nD = thisReader.update();
	if(nD)lagCount = 80;
	currentLayer.update();
	
	if(!isFixed && !pauseFollow){
		
		viewPort.x = thisReader.getPos().x + trans.x;
		viewPort.y = thisReader.getPos().y + trans.y;
		
		if(lagCount > 1){
			trans -= thisReader.getIncrement() * (float)(lagCount/80.0f)  * thisReader.getDirection();
			trans += thisReader.getIncrement()  * (float)(lagCount/80.0f) * direction;
			lagCount -= 1;
		}else if(lagCount ==  1){
			direction = thisReader.getDirection();
			lagCount = 0;
		}

		
		if(abs(trans.x) > 2){ //needs some solution for wrapping //but leave for now
			float incr = (trans.x > 0)? max(1.0,trans.x * 0.02):min(-1.0,trans.x * 0.02);
			trans.x -= incr;
		}else{
			trans.x = 0;
		}
		
		if(abs(trans.y) > 2){ //needs some solution for wrapping //but leave for now
			float incr = (trans.y > 0)? max(1.0,trans.y * 0.02):min(-1.0,trans.y * 0.02);
			trans.y -= incr;
		}else{
			trans.y = 0;
		}
		
		//cout << trans << endl;
		
	}
	
	//for rotation.
	
	if(abs(rotZ - targetZ) > 2){
		
		if(rotZ < targetZ){ 
			rotZ += 1;
		}else{
			rotZ -= 1;
		}
		
	}else{
		rotZ = targetZ;
	}
	
	moduloViewPort();
	
	if(!isMouseDown){
		
		if(mouseMode == MODE_ADD_BLIP || mouseMode == MODE_ADD_TRACK){
			currentLayer.selectSomething(getWorldCoordinate(ofVec2f(mouseX, mouseY)));
		}
		
	}
	
	updateDummyViews();
	
}

void testApp::updateDummyViews(){
	
	ofVec2f t_dims = currentLayer.getDims();
	
	ofxRotRect rotPort(viewPort,rotZ);
	ofRectangle b_rect = rotPort.getBoundingRect();
	
	
	float y_over = viewPort.y + b_rect.height/2.0f - t_dims.y/2.0f;
	float y_under = -viewPort.y + b_rect.height/2.0f - t_dims.y/2.0f;
	float x_over = viewPort.x + b_rect.width/2.0f - t_dims.x/2.0f;
	float x_under = -viewPort.x + b_rect.width/2.0f - t_dims.x/2.0f;
	
	dummy_views.clear();
	ofRectangle dummy_view(viewPort.x,viewPort.y,b_rect.width,b_rect.height);
	
	//theres a better optimisation for this 
	
	if( y_over > 0 ){ 
		dummy_view.y = -t_dims.y + viewPort.y;
		dummy_views.push_back(dummy_view);
	}
	if( y_under > 0 ){ 
		dummy_view.y = t_dims.y +  viewPort.y;
		dummy_views.push_back(dummy_view);
	}
	
	dummy_view.set(viewPort.x,viewPort.y,b_rect.width,b_rect.height);
	
	if( x_over > 0 ){ 
		dummy_view.x = -t_dims.x + viewPort.x;
		dummy_views.push_back(dummy_view);
	}
	if( x_under > 0 ){ 
		dummy_view.x = t_dims.x + viewPort.x;
		dummy_views.push_back(dummy_view);
	}
	
	if( x_over > 0 && y_over > 0){
		dummy_view.y = -t_dims.y + viewPort.y;
		dummy_view.x = -t_dims.x + viewPort.x;
		dummy_views.push_back(dummy_view);
	}
	
	if( x_over > 0 && y_under > 0){
		dummy_view.x = -t_dims.x + viewPort.x;
		dummy_view.y = t_dims.y +  viewPort.y;
		dummy_views.push_back(dummy_view);
	}
	
	if( x_under > 0 && y_under > 0){
		dummy_view.y = t_dims.y +  viewPort.y;
		dummy_view.x = t_dims.x + viewPort.x;
		dummy_views.push_back(dummy_view);
	}
	
	if( x_under > 0 && y_over > 0){
		dummy_view.x = t_dims.x + viewPort.x;
		dummy_view.y = -t_dims.y + viewPort.y;
		dummy_views.push_back(dummy_view);
	}
	
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(255);
	
	glPushMatrix();
	glTranslatef(viewPort.width/2, viewPort.height/2, 0);
	glRotatef(rotZ,0,0,1);
	glRotatef(rotY,0,1,0);
	glTranslatef(-viewPort.width/2, -viewPort.height/2, 0);
	
	ofxRotRect rotPort(viewPort,rotZ);
	ofRectangle b_rect = rotPort.getBoundingRect();
	ofRectangle t(viewPort.x, viewPort.y, b_rect.width, b_rect.height);
	
	currentLayer.draw(t);
	for(int i = 0; i < dummy_views.size(); i ++){currentLayer.draw(dummy_views[i], true);}
	
	thisReader.draw(t);
	for(int i = 0; i < dummy_views.size(); i ++){thisReader.draw(dummy_views[i]);}
	
	glPopMatrix();
	
	ofSetColor(0);
	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate(),2), 900,20);
	ofDrawBitmapString("mode: " + getModeString(mouseMode), 20,20);
	ofDrawBitmapString("blipPreset: " + presets[selectedPreset[1]][selectedPreset[0]].getName(), 400,20);
	ofDrawBitmapString("readerMode: " + thisReader.getModeString(), 700,20);
	ofEnableAlphaBlending();
	ofSetColor(0, 20);
	ofFill();
	ofRect(0,0,ofGetScreenWidth(),30);
	ofDisableAlphaBlending();
	
	ofSetColor(0);
	
	if(isMouseDown && currentAction == ACTION_ADD_BLIP){
		ofLine(mouse_a,mouse_b);
		ofVec2f dir(mouse_b - mouse_a);
		dir.normalize();
		ofDrawBitmapString(currentLayer.getSM()->getPreviewParams(), mouse_b + dir * 20);
	}
	
}


string testApp::getModeString(e_mouseMode temp){
	
	switch(temp){
			
		case MODE_DRAG:return "drag";break;
		case MODE_ADD_BLIP:return "add_blip";break;
		case MODE_ADD_TRACK:return "add_track";break;
		default:return "none";break;
			
	}
	
}

void testApp::moduloViewPort(){
	
	ofVec2f t_dims = currentLayer.getDims();
	
	if(viewPort.y < -t_dims.y/2 -viewPort.height/2 ){
		mouse_offset.y += viewPort.y;	
		viewPort.y += t_dims.y; 
		mouse_offset.y -= viewPort.y;
		//	vpHist.clear();
	}else if(viewPort.y > t_dims.y/2 + viewPort.height/2 ){
		mouse_offset.y += viewPort.y;	
		viewPort.y -= t_dims.y; 
		mouse_offset.y -= viewPort.y;
		//	vpHist.clear();
	}
	
	if(viewPort.x < -t_dims.x/2 -viewPort.width/2 ){
		mouse_offset.x += viewPort.x;	
		viewPort.x += t_dims.x; 
		mouse_offset.x -= viewPort.x;
		//vpHist.clear();
	}else if(viewPort.x > t_dims.x/2 + viewPort.width/2 ){
		mouse_offset.x += viewPort.x;	
		viewPort.x -= t_dims.x; 
		mouse_offset.x -= viewPort.x;
		//vpHist.clear();
	}
	
	
}



ofVec2f testApp::getWorldCoordinate(ofVec2f t_point){
	
	t_point.x -= ofGetScreenWidth()/2;
	t_point.y -= ofGetScreenHeight()/2;
	
	t_point.rotate(-rotZ);
	
	ofVec2f point(t_point);
	
	point.x += viewPort.x;
	point.y += viewPort.y;
	
	ofVec2f t_dims = currentLayer.getDims();
	
	ofRectangle scr(-t_dims.x/2,-t_dims.y/2,t_dims.x,t_dims.y);
	
	if(!scr.inside(point)){
		
		
		for(int i = 0; i < dummy_views.size(); i++){
			
			
			ofRectangle d;
			d.setFromCenter(-dummy_views[i].x, -dummy_views[i].y, t_dims.x, t_dims.y);
			
			if(d.inside(t_point)){
				
				point = t_point + ofVec2f(dummy_views[i].x, dummy_views[i].y);
			}
			
		}
		
		
	}
	
	return point;
	
}

void testApp::prepPauseFollow(){
	
	if(!isFixed){
		pauseFollow = true;
		trans += thisReader.getPos();
		moduloViewPort();
		viewPort.x = trans.x;
		viewPort.y = trans.y;
	}
	
}

void testApp::startAction(){
	
	ofVec2f p(mouseX,mouseY);
	p.rotate(-rotZ);
	
	if(currentAction == ACTION_DRAG){
		
		prepPauseFollow();
		
		mouse_offset.x = -p.x - viewPort.x;
		mouse_offset.y = -p.y - viewPort.y;
		
	}else if(currentAction == ACTION_ADD_SHORT_TRACK ||currentAction == ACTION_ADD_LONG_TRACK){
		
		prepPauseFollow();
		
		if(!isFixed)pauseFollow = true;
		currentLayer.getSM()->beginTrack(getWorldCoordinate(ofVec2f(mouseX,mouseY)));
		
	}else if(currentAction == ACTION_ADD_BLIP){
		
		prepPauseFollow();
		selectedPreset[1] = (isOptionKey)? buttonMode + 2: buttonMode;
		currentLayer.getSM()->beginBlip(getWorldCoordinate(ofVec2f(mouseX,mouseY)), presets[selectedPreset[1]][selectedPreset[0]]);
		
	}
	
}

void testApp::continueAction(ofVec2f t_dir){
	
	ofVec2f p(mouseX,mouseY);
	p.rotate(-rotZ);
	
	if(currentAction == ACTION_DRAG){
		
		viewPort.x = -mouse_offset.x - p.x;
		viewPort.y = -mouse_offset.y - p.y;
		
		trans.x = -mouse_offset.x - p.x;
		trans.y = -mouse_offset.y - p.y;
		
	}else if(currentAction == ACTION_ADD_SHORT_TRACK){
		
		t_dir.rotate(-rotZ);
		currentLayer.getSM()->calcTrack(getWorldCoordinate(ofVec2f(mouseX,mouseY)),t_dir,0);
		
	}else if(currentAction == ACTION_ADD_LONG_TRACK){
		
		t_dir.rotate(-rotZ);
		currentLayer.getSM()->calcTrack(getWorldCoordinate(ofVec2f(mouseX,mouseY)),t_dir,1);
		
	}else if(currentAction == ACTION_ADD_BLIP){
		
		currentLayer.getSM()->calcBlip(getWorldCoordinate(ofVec2f(mouseX,mouseY)),t_dir);
		if(t_dir.length() > 1){
			presets[selectedPreset[1]][selectedPreset[0]].setUserVals(t_dir.length(), abs(t_dir.angle(ofVec2f(0,1)))); //store the current user vals in the preset
		}
		
	}
}

void testApp::endAction(){
	
	if(currentAction == ACTION_DRAG){
		
		if(!isFixed){
			pauseFollow = false;
			trans -= thisReader.getPos();
		}
		
	}else if(currentAction == ACTION_ADD_SHORT_TRACK || currentAction == ACTION_ADD_LONG_TRACK){
		
		isPreview = false;
		if(!isFixed){
			pauseFollow = false;
			trans -= thisReader.getPos();
		}
		currentLayer.getSM()->endTrack();
		
	}else if(currentAction == ACTION_ADD_BLIP){
		
		isPreview = false;
		if(!isFixed){
			pauseFollow = false;
			trans -= thisReader.getPos();
		}
		
		currentLayer.getSM()->endBlip();
		
	}
	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	
	for(int i = 1; i < MODE_COUNT; i ++){
		if(key == 48 + i ){
			mouseMode = e_mouseMode(i);
			currentLayer.deselectAll();	
		}
	}
	
	if(key == ' '){
		
		//vpHist.clear();
		
		if(isFixed){
			trans -= thisReader.getPos();
		}else{
			trans += thisReader.getPos();
			moduloViewPort();
			viewPort.x = trans.x;
			viewPort.y = trans.y;
		}
		
		isFixed = !isFixed;
	}
	
	
	if(key == 9)isOptionKey = true;
	if(key == OF_KEY_UP)selectedPreset[0] = min(selectedPreset[0] + 1, (int)presets[0].size() - 1);
	if(key == OF_KEY_DOWN)selectedPreset[0] = max(selectedPreset[0] - 1, 0);
	if(key == 'r' || key == 'R')thisReader.incrementMode();
	
	if(key == 'f')ofToggleFullscreen();
	if(key == 's')currentLayer.toggleScreenData();
	if(key == 'n')currentLayer.getSM()->toggleNodeData();
	if(key == 't')currentLayer.getSM()->toggleTrackData();
	if(key == 'b')currentLayer.getSM()->toggleBlipData();
	
	if(key == 'z'){targetZ = (targetZ == 0)? -90 : 0;}
	if(key == 'y'){rotY += 1; fmod(rotY,360);}
	
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
	if(isOptionKey)isOptionKey = false;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
	
}



//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	isMouseDown = true;
	mouse_a.set(x,y);
	mouse_b.set(mouse_a);
	
	switch (button) {
		case 0:buttonMode = 0;break;
		default:buttonMode = 1;break;
	}
	
	if(mouseMode == MODE_DRAG){
		currentAction = ACTION_DRAG;
	}else if(mouseMode == MODE_ADD_BLIP){
		currentAction =  ACTION_ADD_BLIP;
	}else if(mouseMode == MODE_ADD_TRACK){
		currentAction = (button == 0 )? ACTION_ADD_SHORT_TRACK: ACTION_ADD_LONG_TRACK;
	}
	
	startAction();
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
	mouse_b.set(x,y);
	ofVec2f dir = mouse_b - mouse_a;
	continueAction(dir);
	
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
	endAction();
	
	isMouseDown = false;
	currentAction = ACTION_NONE;
	
	mouse_a.set(0,0);
	mouse_b.set(0,0);
}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	
}

void testApp::exit(){
	
	ofxOscMessage m;
	m.setAddress("/init");
	sender.sendMessage(m);
	
}
