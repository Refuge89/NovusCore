#include <iostream>

#include <Config\ConfigHandler.h>
#include <Database/DatabaseConnector.h>
#include <Utils/DebugHandler.h>

int main()
{
	/*if (!ConfigHandler::Setup("worldserver_configuration.json"))
	{
		std::getchar();
		return 0;
	}*/
	InitDebugger(PROGRAM_TYPE::World);

	/*NC_LOG_MESSAGE("This is a message");
	NC_LOG_DEPRECATED("This is a deprecation");
	NC_LOG_WARNING("This is a warning");
	NC_LOG_ERROR("This is a error");
	NC_LOG_FATAL("This is a fatal error");*/

	DatabaseConnector::Setup("127.0.0.1", "root", "");

	// Owning a connector
	/*std::unique_ptr<DatabaseConnector> connector;
	if (!DatabaseConnector::Create(DATABASE_TYPE::AUTHSERVER, connector))
	{
		std::cout << "Connecting to database failed!\n";
		return -1;
	}*/

	// Borrowing a connector, first inside of a scope, this calls the constructor
	std::shared_ptr<DatabaseConnector> connector;
	if (!DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, connector))
	{
		std::cout << "Connecting to database failed!\n";
		return -1;
	}

	PreparedStatement sql("SELECT * FROM accounts WHERE id={i} AND name={s}");
	sql.Bind(1).Bind("Pursche");

	amy::result_set results;
	//if (!connector->Query("SELECT * FROM accounts", results)) // Non prepared statement
	if (!connector->Query(sql, results)) // Prepared statement
	{
		std::cout << "Querying the database failed!\n";
		return -1;
	}

	DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, "SELECT * FROM accounts WHERE id=1", [](amy::result_set rs, DatabaseConnector& connector) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Buzz!\n";
	});

	// Also callable through a connector like this:
	/*connector->QueryAsync("SELECT * FROM accounts WHERE id=1", [](amy::result_set rs, DatabaseConnector& connector) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Buzz!\n";
	});*/

	std::cout << "Fizz ";
	std::this_thread::sleep_for(std::chrono::seconds(2));

	std::cout
		<< "Affected rows: " << results.affected_rows()
		<< ", field count: " << results.field_count()
		<< ", result set size: " << results.size()
		<< std::endl;

	const auto& fields_info = results.fields_info();

	for (const auto& row : results) {
		std::cout
			<< fields_info[0].name() << ": " << row[0].as<amy::sql_bigint>() << ", "
			<< fields_info[1].name() << ": " << row[1].as<std::string>() << ", "
			<< fields_info[2].name() << ": " << row[2].as<std::string>() << ", "
			<< fields_info[3].name() << ": " << row[3].as<amy::sql_datetime>() // TODO: This date is off by 1 month, pls fix
			<< std::endl;
	}

	return 0;
}