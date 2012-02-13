/*
 *  flipper.cpp
 *  soundCircuit
 *
 *  Created by Simon Katan on 13/02/2012.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "flipper.h"

flipper::flipper():baseBlipDraw(){
	
	for(int i = 0; i < 3; i++)pivot[i] = 0;
	
}


void flipper::update(){	
	
	testRect.setFromCenter(0, 0, length, params[1]);	
	setWrapData(ofVec2f(length/2,9),angle);
	
	float swing = (postVal > 0) ? decayRatio * envVal + (1-decayRatio) * postVal: envVal;
	
	if(isActive){
		
		if(swing > 0.95){
			isDecay = true;
		}
		
		if(isDecay && swing < 1){
			for(int i = 0; i < 3; i++){
				if(params[6+i] > 0){
					pivot[i] = 360.0f * floor(params[6+i]) *  (1-swing*swing*swing);
				}
				
			}
		}else{
			for(int i = 0; i < 3; i++){
				pivot[i] += 6 * params[6+i] * swing; 
				pivot[i] = fmod(pivot[i], 360.0f);
			}
		}
	
	}else{
		for(int i = 0; i < 3; i++)pivot[i] = 0;
		isDecay = false;
	}
	
	
}

void flipper::draw(int t_wrap){
	
	
	glPushMatrix();
	
	if(t_wrap == 0){
		glTranslatef(centre.x, centre.y, 0);
	}else if(t_wrap == 1){
		glTranslatef(wrapCoords.x, centre.y, 0); 
	}else if(t_wrap == 2){
		glTranslatef(centre.x, wrapCoords.y, 0); 
	}
	
	glRotatef(angle, 0, 0, 1);
	
	glRotatef(pivot[0], 1, 0, 0);
	glRotatef(pivot[1], 0, 1, 0);
	glRotatef(pivot[2], 0, 0, 1);
	
	glTranslatef(0, params[1] * params[0]/2, 0);
	
	ofSetColor(0);
	ofNoFill();
	ofSetRectMode(OF_RECTMODE_CENTER);
	ofRect(0, 0, length, params[0]);
	ofSetRectMode(OF_RECTMODE_CORNER);
	
	
	glPopMatrix();
	
	
}


vector<paramAttributes> flipper::getParamDefs(){
	
	vector<paramAttributes> def;
	
	paramAttributes height, trackOffset, shapeType, x_rot, y_rot, z_rot, x_speed, y_speed, z_speed;
	height.name = "height"; height.min_val = 10; height.max_val = 100; height.abs_value = 20;
	trackOffset.name = "trackOffset"; trackOffset.min_val = -1; trackOffset.max_val = 1; trackOffset.abs_value = 0;
	shapeType.name = "shapeType"; shapeType.min_val = 0; shapeType.max_val = 3; shapeType.abs_value = 0;
	x_rot.name = "x_rot"; x_rot.min_val = 0; x_rot.max_val = 360; x_rot.abs_value = 0;
	y_rot.name = "y_rot"; y_rot.min_val = 0; y_rot.max_val = 360; y_rot.abs_value = 0;
	z_rot.name = "z_rot"; z_rot.min_val = 0; z_rot.max_val = 360; z_rot.abs_value = 0;
	x_speed.name = "x_speed"; x_speed.min_val = 0; x_speed.max_val = 10; x_speed.abs_value = 0;
	y_speed.name = "y_speed"; y_speed.min_val = 0; y_speed.max_val = 10; y_speed.abs_value = 0;
	z_speed.name = "z_speed"; z_speed.min_val = 0; z_speed.max_val = 10; z_speed.abs_value = 0;

	def.push_back(height);
	def.push_back(trackOffset);
	def.push_back(shapeType);
	def.push_back(x_rot); //3
	def.push_back(y_rot); //4
	def.push_back(z_rot); //5
	def.push_back(x_speed);
	def.push_back(y_speed);
	def.push_back(z_speed);
	
	return def;
	
}