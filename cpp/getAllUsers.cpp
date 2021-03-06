/*
 * Original Author: Liam Foster
 * Date of Creation: 9/10/2016
 * Filename: getAllUsers.cpp
 * Usage and implementation: Searches database for a user and returns all user information, sorted alphabetically.
 */

/* JSON FORMAT:

{
	"status": "found",
	"users": {
		"id":		...,
		"username":	...,
		"displayname":	...,
		"email":	...,
		"isReporter":	...,
		"isReviewer":	...,
		"isTriager":	...,
		"isDeveloper":	...,
		"isAdmin":	...,
		"reputation":	...,
		"active":	...
	}
}

or...

{
	"status": "not found"
}

*/

/* HOW TO COMPILE FROM THE firebug DIRECTORY (this assumes you have installed json.hpp to the /usr/include/json directory) (this also assumes that you are using g++ version 4.9 or higher):

g++ -std=c++11 -Wall -I/usr/include/cppconn -I/usr/include/json -o web/cgi-bin/getAllUsers.cgi cpp/getAllUsers.cpp -L/usr/lib -lmysqlcppconn -lcgicc
*/

/* Standard C++ includes */
#include <iostream>
#include <string>

/* Connector/C++ includes */
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

/* CGI Includes */
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

/* JSON Include */
#include "json.hpp"

int main(void)
{
	cgicc::Cgicc cgi;
	nlohmann::json retJ;	

	std::cout << cgicc::HTTPHTMLHeader() << std::endl;

	try {
		sql::Driver *driver;
		sql::Connection *conn;
		sql::PreparedStatement *prepStmt;
		sql::ResultSet *res;

		// Create the connection
		driver = get_driver_instance();
		conn = driver->connect("localhost", "web", "VerySecure");

		// Connect to the database required
		conn->setSchema("FIREBUG");

		// Use a prepared statement to prevent SQL injection
		// Selects the bug data which matches the id supplied by POST
		prepStmt = conn->prepareStatement( "SELECT * FROM user" );

		// Execute the query
		res = prepStmt->executeQuery();

		// Convert results into JSON object
		if (!res->next())
		{
			// User has not been found, build appropriate empty JSON object
			retJ["status"] = "not found";
		}
		else
		{
			// User has been found, build appropriate JSON object
			res->beforeFirst(); // Make sure we are at the start of the results list
			retJ["status"] = "found";

			while (res->next())
			{
				retJ["user"].push_back( { {"id", res->getInt("id")}, {"username", res->getString("username")}, {"displayname", res->getString("displayname")}, {"email", res->getString("email")},  {"isReporter", res->getInt("isReporter")}, {"isReviewer", res->getInt("isReviewer")}, {"isTriager", res->getInt("isTriager")}, {"isDeveloper", res->getInt("isDeveloper")}, {"isAdmin", res->getInt("isAdmin")}, {"reputation", res->getDouble("reputation")}, {"active", res->getInt("active")} } ); 
			
			}
		}

		// Return the object to the page
		std::cout << retJ << std::endl;
	}
		catch(sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
  		std::cout << " (MySQL error code: " << e.getErrorCode();
  		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
	catch(...) {
		std::cout << "Something went wrong with the SQL or the JSON formatting!\nDumping the data set:\n";
		std::cout << retJ.dump(4) << std::endl;
	}

	return 0;
}
