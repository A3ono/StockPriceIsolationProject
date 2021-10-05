//Alexandra Medina

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_set>
#include <map>
#include <chrono>
#include <ctime>

using namespace std;

// Structure modeling a CSV line
struct Data
{
	string ticker_name;
	// string date;	// date is constant to the whole stock;
	double open_price;
	// double high_price;	
	// double low_price;	
	double close_price;
	// int volume;	
};

int main()
{
	const auto start_time = chrono::system_clock::now(); // Setup starting time of the program

	const string input_path = "C:\\Ticker Input Files";
	const string output_path = "C:\\Ticker Output Files";
	const string open_prices_path = output_path + "\\" + "Open Prices.csv";
	const string close_prices_path = output_path + "\\" + "Close Prices.csv";
	

//+++++++++++++++++++++++++ STOCK DATA VECTOR ++++++++++++++++++++++++++++++++

	vector<map<string, Data>> data;	// To store all the stocks data
	vector<string> stock_dates; // To store the dates of each stock
	for (const auto& p : filesystem::directory_iterator(input_path)) // Browse The input directory files
	{
		ifstream input_file(p.path());
		string line;
		map<string, Data> file_data;	// To store the current file data
		string file_date; // To store the current file date
		while (getline(input_file, line))	// Read file line by line
		{
			Data entry;
			size_t offset1 = 0, offset2;
			string token;

			// Extract Ticker Name
			offset2 = line.find(",", offset1);
			entry.ticker_name = line.substr(offset1, offset2 - offset1);
			offset1 = offset2 + 1;
			
			// Extract Ticker Date
			offset2 = line.find(",", offset1);
			file_date = line.substr(offset1, offset2 - offset1);
			offset1 = offset2 + 1;

			// Extract Ticker Open price
			offset2 = line.find(",", offset1);
			entry.open_price = stod(line.substr(offset1, offset2 - offset1));
			offset1 = offset2 + 1;

			// Extract Close Price
			offset2 = line.find(",", offset1);
			entry.close_price = stod(line.substr(offset1, offset2 - offset1));
			offset1 = offset2 + 1;

			// Extract High price
			offset2 = line.find(",", offset1);
			// entry.high_price = stod(line.substr(offset1, offset2 - offset1));
			offset1 = offset2 + 1;

			// Extract Low Price
			offset2 = line.find(",", offset1);
			// entry.low_price = stod(line.substr(offset1, offset2 - offset1));
			offset1 = offset2 + 1;

			// Extract Volume
			// offset2 = line.find(",", offset1);
			// entry.volume = stoi(line.substr(offset1, offset2 - offset1));
			// offset1 = offset2 + 1;

			file_data[entry.ticker_name] = entry;	 // Append the current entry to the file entries
		}
		data.push_back(file_data); // Append the current data file to the complete data
		stock_dates.push_back(file_date); // Append the date of the current file
		input_file.close();
	}

//+++++++++++++++++++++ COMPUTE STOCK AVAILABILITY ++++++++++++++++++++++++++++++++

	
	const auto input_files_count = data.size();	// Number of input files

	// Compute all the available tickers
	unordered_set<string> all_tickers;	// To store all the existing tickers names
	for (const auto& file_data : data)
	{
		for (const auto& entry : file_data)
			all_tickers.insert(entry.first); // entry.first == ticker_name
	}

	// Compute Tickers that aren't available in every file
	unordered_set<string> tickers_not_everywhere;	// To store all the tickers that aren't present in every file
	for (const string& ticker : all_tickers)	// Browse ticker by ticker
	{
		for (const auto& file_data : data)	// Browse all current file data
		{
			if (file_data.find(ticker) == end(file_data))	// If ticker isn't available, add it to the "not present in every file" tickers set
				tickers_not_everywhere.insert(ticker);
		}
	}

	// Compute tickets that are available everywhere
	unordered_set<string> tickers_everywhere;
	for (const auto& ticker : all_tickers)
	{
		if (tickers_not_everywhere.find(ticker) == end(tickers_not_everywhere))
			tickers_everywhere.insert(ticker);
	}
	
//+++++++++++++++++++++++++++ OUTPUT ++++++++++++++++++++++++++++++++


	// Output results
	
	filesystem::create_directories(output_path); // Create Directory output directory
	ofstream open_prices_file(open_prices_path); // Open prices output file
	ofstream close_prices_file(close_prices_path); // Close prices output file
	
	// Print header lines
	open_prices_file << "\"Date\"";
	close_prices_file << "\"Date\"";
	for (const auto& ticker : tickers_everywhere)
	{
		open_prices_file << "," << ticker; // ,Ticker_name1 ...
		close_prices_file << "," << ticker;
	}
	open_prices_file << '\n';	// New line
	close_prices_file << '\n';

	// Prints the rest of the data
	for (size_t i = 0; i < data.size(); ++i) // Browse input files data 1 by 1
	{
		const auto& stock = data[i];
		const auto& date = stock_dates[i];
		open_prices_file << date;	// Prints the date in the first column
		close_prices_file << date; 

		for (const auto& ticker : tickers_everywhere) // Browse "tickers available in every file"
		{
			// Search for the ticker in the stock entries
			const auto& entry = stock.at(ticker);
			open_prices_file << "," << entry.open_price;
			close_prices_file << "," << entry.close_price;
		}
		open_prices_file << '\n';	// New line
		close_prices_file << '\n';
	}
	
	// Close files
	open_prices_file.close();
	close_prices_file.close();
	
//+++++++++++++++++++++++++++ TIME ++++++++++++++++++++++++++++++++


	// Calculates End time and Elapsed time
	const auto end_time = chrono::system_clock::now(); // End time of the program
	const auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count(); // elapsed time in milliseconds
	const auto start_time_t = chrono::system_clock::to_time_t(start_time); // convert to time_t
	const auto end_time_t = chrono::system_clock::to_time_t(end_time); // convert to time_t

	// Prints time and count files
	cout << ctime(&start_time_t) << '\n' // Start time
		<< "Number of files = " << input_files_count << '\n' // Count input files
		<< ctime(&end_time_t) << '\n' // End Time
		<< "Elapsed Time = " << std::setprecision(3) << std::fixed << elapsed_time/1e3 << '\n'; // Formats in 3-digit precision
}
