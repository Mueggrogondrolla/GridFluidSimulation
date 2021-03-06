/*******************************************************************************
 *  ____   ___  ____
 * / ___) / __)(  __)
 * \___ \( (_ \ ) _)
 * (____/ \___/(____) - Simple Game Engine, Version 2
 *
 * Copyright (c) 2018, 2019 Roman Divotkey,
 * Univ. of Applied Sciences Upper Austria. All rights reserved.
 *
 * This file is subject to the terms and conditions defined in file
 * 'LICENSE', which is part of this distribution.
 *
 * THIS CODE IS PROVIDED AS EDUCATIONAL MATERIAL AND NOT INTENDED TO ADDRESS
 * ALL REAL WORLD PROBLEMS AND ISSUES IN DETAIL.
*******************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Sge.h>
#include "TestGridRenderState.h"


#define APP_NAME		"GridFluidSimulation"
#define APP_TITLE		"GridFluidSimulation"
#define	CPRT_HOLDER		"Foo Games Inc."
#define MAJOR_NUMBER    1
#define MINOR_NUMBER    0
#define REVISION_NUMBER 0

/**
 * Adds the mains states of this application and configures the state transitions.
 *
 * @param sm	the state machine used to add the states
 */
void addMainStates(sge::IStateMachine & sm) {
	// TODO: add application main states.

	// sm.addState(std::make_shared<SplashStatePlum>(), "Splash");
	// sm.addState(std::make_shared<MenuStatePlum>(), "Menu");
	// sm.addState(std::make_shared<PlayStatePlum>(), "Play");
	sm.addState(std::make_shared<sge::EndStatePlum>(), "End");
	sm.addState(std::make_shared<TestGridRenderState>(), "GridRenderTest");

	sm.setStartState("GridRenderTest");

	// TODO: add state transitions

	// sm.setStartState("Splash");
	// sm.addTransition("Splash", "Menu", "EndOfState");
	// sm.addTransition("Menu", "Play", "PlaySelected");
	// sm.addTransition("Play", "Menu", "EndOfState");
	//sm.addTransition("Menu", "End", "EndOfState");
}

/**
 * Adds additional services required to exists during the entire application lifetime.
 *
 * @para root	the root Plum where to add services.
 */
void addServices(sge::Plum & root) {

	// TOTO: add services.

	//root.addChild(std::make_shared<MyServicePlum>());
}


/**
 * Creates and initializes the application.
 *
 * @param builder	the application builder used to create the application
 * @param cml		the command line arguments
 * @return the fully initialize application
 */
std::shared_ptr<sge::IApplication> initializeApp(sge::ISgeApplicationBuilder & builder, const std::string & cml) {

	auto clap = builder.buildClap();
	try {
		clap->parse(cml);
	}
	catch (std::runtime_error & e) {
		throw sge::CommandLineException(e.what(), clap->getOptionString());
	}

	// Build application with application builder.
	auto app = builder
		.appName(APP_NAME)
		.copyrightHolder(CPRT_HOLDER)
		.copyrightDate("2019")
		.version(sge::Version(MAJOR_NUMBER, MINOR_NUMBER, REVISION_NUMBER))
		.build(*clap);

	addServices(app->getRoot());
	addMainStates(app->getRoot().usePlum<sge::IStateMachine>());

	return app;
}

/**
 * The main Windows entry point.
 * This method implements the code required to run an Windows executable.
 *
 * @param hInstance		handle to an instance, uses this value to identify the executable
 * @param hPrevInstance	has no meaning, used in 16-bit Windows, always zero
 * @param pCmdLine		contains the command-line arguments as a ASCII string
 * @param nCmdShow		whether the application should be minimized, maximized, or shown normally
 * @return the status code of this application (zero indicates no error)
 */
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Let's keep application outside try block in order to delay
	// destruction of application until error dialog is done.
	// Destruction of the application in case of an erroneous condition
	// might cause other exceptions or problems which might in turn
	// prevent the error dialog to show up.

	// Create and initialize application.
	std::shared_ptr<sge::IApplication> app;

	try {
		// Build and run the application.
		auto app = initializeApp(*sge::createSgeAppBuilder(), sge::StringUtils::utf16ToUtf8(pCmdLine));
		app->run();
	}
	catch (const std::logic_error & e) {
		sge::reportBadDeveloper(e.what());
		exit(-1); // force exit, avoid application cleanup
	}
	catch (std::runtime_error& e) {
		sge::reportRuntimeError(e.what());
		exit(-1); // force exit, avoid application cleanup
	}
	catch (sge::CommandLineException & e) {
		sge::reportCommandLineError(e);
		exit(-1);
	}

	return 0;
}
