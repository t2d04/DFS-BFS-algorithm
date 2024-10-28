/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
#include <stack>
#include <queue>
using namespace std;
Maze mz;
Maze Mark;
stack<Hst> history;
stack<Hst> path;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isBFS = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS",false,false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if(title == "Open") {
		readFile();
	}
	if(title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if(title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			DFS();
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
		
	}

	/*
	원본 내용
	if(title == "Show BFS") {
		doTopmost(bChecked);
		
	}
	*/

	if(title == "Show BFS") {
		
		bTopmost = true;
		doTopmost(bChecked);

		if (isOpen)
		{
			BFS();
			bShowInfo = bChecked;
		}
		else
			cout << "You must open file first" << endl;

	}

	if(title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if(title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	
	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here

	int linesize = 30;

	ofDrawLine(0, 0, linesize * mz.cnt_n, 0);
	ofDrawLine(0, 0, 0, linesize * mz.cnt_m);

	for (int i = 0; i < mz.cnt_m; i++) {

		for (int j = 0; j < mz.cnt_n; j++) {

			if (mz.arr[i][j].S == NULL)
				ofDrawLine(linesize * j, linesize * (i + 1), linesize * (j + 1), linesize * (i + 1));
			if (mz.arr[i][j].E == NULL)
				ofDrawLine(linesize * (j + 1), linesize * i, linesize * (j + 1), linesize * (i + 1));
		}
	}

	if (isdfs)
	{
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			cout << "You must open file first" << endl;
	}

	if (isBFS)
	{

		if (isOpen)
			bfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	if(bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight()-20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if(bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else { 
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU)); 
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if(bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if(bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	// Escape key exit has been disabled but it can be checked here
	if(key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if(bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);
			
			// Maze 동적할당 전 각 줄을 Cell 형 1차원 배열로 저장하고, 해당 포인터들을 tmp_maze에 저장

			// TO DO
			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다

			auto lines = ofSplitString(buffer, "\n");
			mz.cnt_m = (lines.size() - 1)/2;
			mz.arr = (Cell**)malloc(sizeof(Cell*) * mz.cnt_m);
			string tmp = lines.front();
			mz.cnt_n = (tmp.length() - 1) / 2;

			for (int i = 0; i < mz.cnt_m; i++)
				mz.arr[i] = (Cell*)malloc(sizeof(Cell) * mz.cnt_n);


			for (int i = 0; i < mz.cnt_m; i++) {

				// 위의 세 줄을 tmp1, 2, 3에 저장
				string tmp1 = lines[2 * i];
				string tmp2 = lines[2 * i + 1];
				string tmp3 = lines[2 * i + 2];

				for (int j = 0; j < mz.cnt_n; j++) {

					if (tmp1[2 * j + 1] == ' ')
						mz.arr[i][j].N = &(mz.arr[i - 1][j]);
					else mz.arr[i][j].N = NULL;
					if (tmp2[2 * j] == ' ')
						mz.arr[i][j].W = &(mz.arr[i][j - 1]);
					else mz.arr[i][j].W = NULL;
					if (tmp2[2 * j + 2] == ' ')
						mz.arr[i][j].E = &(mz.arr[i][j + 1]);
					else mz.arr[i][j].E = NULL;
					if (tmp3[2 * j + 1] == ' ')
						mz.arr[i][j].S = &(mz.arr[i + 1][j]);
					else mz.arr[i][j].S = NULL;
				}
			}
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::freeMemory() {

	//TO DO
	// malloc한 memory를 free해주는 함수

	for (int i = 0; i < mz.cnt_m; i++)
		free(mz.arr[i]);
	free(mz.arr);
}


bool ofApp::DFS()//DFS탐색을 하는 함수
{
	//TO DO
	//DFS탐색을 하는 함수 ( 3주차)
	//stack : history
	isdfs = true;
	short FLAG = 0;
	Hst current;
	current.i = 0;
	current.j = 0;
	current.dir = 0;

	char prev = 0;

	//E == 1, S == 2; W == 3, N == 4;

	while (!history.empty())
		history.pop();
	while (!path.empty())
		path.pop();

	while (!FLAG) {

		//pop을 통해 가져온 경우
		if (current.dir) {

			switch (current.dir) {

			case 1:
				current.dir++;
				//직전 위치로 돌아가는 경우를 prev를 이용해 배제
				if (mz.arr[current.i][current.j].S && prev != 4) {
					//stack에 current 삽입, prev와 current 업데이트
					path.push(current);
					history.push(current);
					prev = current.dir;
					current.i++;
					current.dir = 0;
					break;
				}

			case 2:
				current.dir++;
				if (mz.arr[current.i][current.j].W && prev != 1) {
					path.push(current);
					history.push(current);
					prev = current.dir;
					current.j--;
					current.dir = 0;
					break;
				}

			case 3:
				current.dir++;
				if (mz.arr[current.i][current.j].N && prev != 2) {
					path.push(current);
					history.push(current);
					prev = current.dir;
					current.i--;
					current.dir = 0;
					break;
				}

			default:
				if (!path.empty()) {
					//다시 직전 위치로 돌아가는 방향으로 dir 업데이트 후 history에 push
					switch (prev) {
					case 1:
						current.dir = 3;
						break;
					case 2:
						current.dir = 4;
						break;
					case 3:
						current.dir = 1;
						break;
					case 4:
						current.dir = 2;
						break;
					}

					history.push(current);
					//current를 직전 위치로 업데이트
					current = path.top();
					path.pop();
					
					//prev 전 전 위치로 업데이트
					if (!path.empty())
						prev = path.top().dir;
					else
						prev = 0;
				}

				//stack이 비어있어 더 뒤로 갈 수 없는 경우 탐색 종료
				else {
					FLAG = -1;
					current.dir = 0;
					history.push(current);
				}
			}

		}

		//새롭게 도착한 곳일 경우
		else {

			if (mz.arr[current.i][current.j].E && prev != 3) {
				current.dir = 1;
				path.push(current);
				history.push(current);
				current.j++;
				current.dir = 0;
				prev = 1;
			}

			else {
				if (mz.arr[current.i][current.j].S && prev != 4) {
					current.dir = 2;
					path.push(current);
					history.push(current);
					current.i++;
					current.dir = 0;
					prev = 2;
				}

				else {
					if (mz.arr[current.i][current.j].W && prev != 1) {
						current.dir = 3;
						path.push(current);
						history.push(current);
						current.j--;
						current.dir = 0;
						prev = 3;
					}

					else {
						if (mz.arr[current.i][current.j].N && prev != 2) {
							current.dir = 4;
							path.push(current);
							history.push(current);
							current.i--;
							current.dir = 0;
							prev = 4;
						}

						else {
							if (!path.empty()) {
								switch (prev) {
								case 1:
									current.dir = 3;
									break;
								case 2:
									current.dir = 4;
									break;
								case 3:
									current.dir = 1;
									break;
								case 4:
									current.dir = 2;
									break;
								}
								history.push(current);
								current = path.top();
								path.pop();

								if (!path.empty())
									prev = path.top().dir;
								else
									prev = 0;
							}

							//더 이상 돌아갈 곳이 없는 경우 
							else {
								FLAG = -1;
								history.push(current);
							}
						}
					}
				}
			}
		}

		//출구를 찾았을 때
		if (current.i == mz.cnt_m - 1 && current.j == mz.cnt_n - 1) {
			FLAG = 1;
			current.dir = 0;
			path.push(current);
			history.push(current);
		}
	}

	//경로를 찾지 못했을 때
	if (FLAG == -1) {
		printf("No possible path\n");
	}

	return false;
}

void ofApp::dfsdraw()
{
	//TO DO 
	//DFS를 수행한 결과를 그린다. (3주차 내용)

	//cell 한 칸의 크기
	int cellsize = 30;

	//history 먼저 출력

	ofSetColor(90);
	ofSetLineWidth(10);

	//current를 가장 최근 노드로 업데이트

	stack<Hst> his_cpy = history;
	Hst current = his_cpy.top();
	his_cpy.pop();

	Hst previous;

	while (!his_cpy.empty()) {

		previous = his_cpy.top();
		his_cpy.pop();

		ofDrawLine(cellsize * (previous.j + 0.5), cellsize * (previous.i + 0.5), cellsize * (current.j + 0.5), cellsize * (current.i + 0.5));

		if (!his_cpy.empty())
			current = previous;
	}

	//path 출력

	ofSetColor(0, 0, 255, 255);
	ofSetLineWidth(10);

	if (!path.empty()) {
		stack<Hst> path_cpy = path;
		current = path_cpy.top();
		path_cpy.pop();

		while (!path_cpy.empty()) {

			previous = path_cpy.top();
			path_cpy.pop();

			ofDrawLine(cellsize * (previous.j + 0.5), cellsize * (previous.i + 0.5), cellsize * (current.j + 0.5), cellsize * (current.i + 0.5));

			if (!path_cpy.empty())
				current = previous;
		}
	}
}

bool ofApp::BFS() {

	while (!path.empty())
		path.pop();

	isBFS = true;

	Mark.cnt_m = mz.cnt_m;
	Mark.cnt_n = mz.cnt_n;
	Mark.arr = (Cell**)malloc(Mark.cnt_m * sizeof(Cell*));
	
	for (int i = 0; i < Mark.cnt_m; i++) {
		Mark.arr[i] = (Cell*)malloc(Mark.cnt_n * sizeof(Cell));

		for (int j = 0; j < Mark.cnt_n; j++) {
			Mark.arr[i][j].N = NULL;
			Mark.arr[i][j].E = NULL;
			Mark.arr[i][j].W = NULL;
			Mark.arr[i][j].S = NULL;
		}
	}
	queue<Hst> history;

	//BFS에서 Hst의 dir은 직전 노드로 연결되는 곳 의미

	Hst current;
	Hst temp;
	current.i = 0;
	current.j = 0;
	current.dir = 0;

	history.push(current);

	short FLAG = -1;

	while (!history.empty()) {

		switch (current.dir) {

		case 1:
			Mark.arr[current.i][current.j].E = &Mark.arr[current.i][current.j + 1];
			Mark.arr[current.i][current.j + 1].W = &Mark.arr[current.i][current.j];
			break;

		case 2:
			Mark.arr[current.i][current.j].S = &Mark.arr[current.i + 1][current.j];
			Mark.arr[current.i + 1][current.j].N = &Mark.arr[current.i][current.j];
			break;

		case 3:
			Mark.arr[current.i][current.j].W = &Mark.arr[current.i][current.j - 1];
			Mark.arr[current.i][current.j - 1].E = &Mark.arr[current.i][current.j];
			break;

		case 4:
			Mark.arr[current.i][current.j].N = &Mark.arr[current.i - 1][current.j];
			Mark.arr[current.i - 1][current.j].S = &Mark.arr[current.i][current.j];
		}

		if (current.i == Mark.cnt_m - 1 && current.j == Mark.cnt_n - 1) {

			FLAG = 0;
			break;
		}

		if (mz.arr[current.i][current.j].E && current.dir != 1) {

			temp.i = current.i;
			temp.j = current.j + 1;
			temp.dir = 3;
			history.push(temp);
		}

		if (mz.arr[current.i][current.j].S && current.dir != 2) {

			temp.i = current.i + 1;
			temp.j = current.j;
			temp.dir = 4;
			history.push(temp);
		}

		if (mz.arr[current.i][current.j].W && current.dir != 3) {

			temp.i = current.i;
			temp.j = current.j - 1;
			temp.dir = 1;
			history.push(temp);
		}

		if (mz.arr[current.i][current.j].N && current.dir != 4) {

			temp.i = current.i - 1;
			temp.j = current.j;
			temp.dir = 2;
			history.push(temp);
		}

		current = history.front();
		history.pop();
	}

	if (FLAG) {
		printf("No possible path\n");
	}

	else {

		//path에 저장
		current.dir = 0;
		path.push(current);
		int prev = 0;

		while (!path.empty()) {

			//pop을 통해 가져온 경우
			if (current.dir) {

				switch (current.dir) {

				case 1:
					current.dir++;
					//직전 위치로 돌아가는 경우를 prev를 이용해 배제
					if (Mark.arr[current.i][current.j].S && prev != 4) {
						//stack에 current 삽입, prev와 current 업데이트
						path.push(current);
						prev = current.dir;
						current.i++;
						current.dir = 0;
						break;
					}

				case 2:
					current.dir++;
					if (Mark.arr[current.i][current.j].W && prev != 1) {
						path.push(current);
						prev = current.dir;
						current.j--;
						current.dir = 0;
						break;
					}

				case 3:
					current.dir++;
					if (Mark.arr[current.i][current.j].N && prev != 2) {
						path.push(current);
						prev = current.dir;
						current.i--;
						current.dir = 0;
						break;
					}

				default:
					if (!path.empty()) {

						//current를 직전 위치로 업데이트
						current = path.top();
						path.pop();

						//prev 전 전 위치로 업데이트
						if (!path.empty())
							prev = path.top().dir;
						else
							prev = 0;
					}
				}
			}

			//새롭게 도착한 곳일 경우
			else {

				if (Mark.arr[current.i][current.j].E && prev != 3) {
					current.dir = 1;
					path.push(current);
					current.j++;
					current.dir = 0;
					prev = 1;
				}

				else {
					if (Mark.arr[current.i][current.j].S && prev != 4) {
						current.dir = 2;
						path.push(current);
						history.push(current);
						current.i++;
						current.dir = 0;
						prev = 2;
					}

					else {
						if (Mark.arr[current.i][current.j].W && prev != 1) {
							current.dir = 3;
							path.push(current);
							history.push(current);
							current.j--;
							current.dir = 0;
							prev = 3;
						}

						else {
							if (Mark.arr[current.i][current.j].N && prev != 2) {
								current.dir = 4;
								path.push(current);
								history.push(current);
								current.i--;
								current.dir = 0;
								prev = 4;
							}

							else {
								if (!path.empty()) {
									current = path.top();
									path.pop();

									if (!path.empty())
										prev = path.top().dir;
									else
										prev = 0;
								}
							}
						}
					}
				}
			}

			//출구를 찾았을 때
			if (current.i == 0 && current.j == 0) {
				FLAG = 1;
				current.dir = 0;
				path.push(current);
				break;
			}
		}
			
	}

	return true;
}

void ofApp::bfsdraw() {

	int cellsize = 30;

	//history 먼저 출력
	//그냥 무식하게 Mark로 해야겠다

	ofSetColor(90);
	ofSetLineWidth(10);

	for (int i = 0; i < Mark.cnt_m; i++) {

		for (int j = 0; j < Mark.cnt_n; j++) {

			if (Mark.arr[i][j].E)
				ofDrawLine(cellsize*(j + 0.5), cellsize*(i + 0.5), cellsize * (j + 1.5), cellsize * (i + 0.5));

			if (Mark.arr[i][j].S)
				ofDrawLine(cellsize * (j + 0.5), cellsize * (i + 0.5), cellsize * (j + 0.5), cellsize * (i + 1.5));
		}
	}

	ofSetColor(0, 0, 255, 255);
	ofSetLineWidth(10);

	if (!path.empty()) {
		stack<Hst> path_cpy = path;
		Hst current = path_cpy.top();
		path_cpy.pop();

		while (!path_cpy.empty()) {

			Hst previous = path_cpy.top();
			path_cpy.pop();

			ofDrawLine(cellsize * (previous.j + 0.5), cellsize * (previous.i + 0.5), cellsize * (current.j + 0.5), cellsize * (current.i + 0.5));

			if (!path_cpy.empty())
				current = previous;
		}
	}

}