/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2012                                               */
/* Johannes Lorenz, Philipp Lorenz                                       */
/* https://github.com/JohannesLorenz/LoQuality                           */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#include "params.h"

int read_params(int p_argc, char *p_argv[])
{
	char c;
	
	int option_index = 0;

	static struct option long_options[] = {
	{"help", 0, 0, 'h'},
	{"version", 0, 0, 'v'},
	{0, 0, 0, 0}
	};
 
	optind=0;

	while(1)
	{
		c=getopt_long_only(p_argc, p_argv, "hv",long_options,&option_index);
 
		if(c==-1)
			break;
 
		switch(c)
		{
   			case 'h':
				/*puts("ODF-Image v0.1 alpha");
				puts("::::::::::::::::::::\n");
				puts("This is a program by the Open Descent Foundation (http://odf.sf.net).");
				puts("You are allowed to modify this program under the conditions of the");
				puts("GNU General Public License, version 3.\n");
				printf("Usage: %s [OAF-file] [options]\n\n", p_argv[0]);
				puts("Options:\n");
				puts("[-h/--help]    - view this text");
				puts("[-v/--version] - print version\n");
				puts("Written and designed by Philipp \"D.Cent\" Lorenz\n");
				puts("This program is based on open source and is for free!");
				puts("Ideas are welcome: descent1@gmx.net\n");
				puts("NOTE: This program is just in development.");
				puts("      Don't expect it to work!");
		
				exit(0);*/
				
                cout << "LoQuality " << globals::LQ_VERSION << endl << endl;
				
				cout << "Project homepage: http://github.com/JohannesLorenz/LoQuality" << endl;
				cout << "You are allowed to modify or redistribute this program under the conditions of the" << endl;
				cout << "GNU General Public License, at least version 3. See 'LICENSE.txt' for more information!" << endl << endl;
                
				cout << "Usage: " << p_argv[0] << endl << endl;
                
				cout << "Options:" << endl;
				cout << "[-h/--help]    - view this text" << endl;
				cout << "[-v/--version] - print version" << endl << endl;
                
				cout << "NOTE: This program is still in development." << endl;
                cout << "      Any ideas or contributions are welcome!" << endl;
		
				return -1;
  
			case 'v':
				//puts("ODF-Image v0.1 alpha; Written and designed by Philipp \"D.Cent\" Lorenz\nLicensed under GPL v3");
				cout << "LoQuality " << globals::LQ_VERSION << " - written by Johannes & Philipp Lorenz" << endl;
                cout << "Licensed under GPL v3 - see LICENSE.txt" << endl;
            
				return -1;
  
		}
	}
	
	return 0;
}
