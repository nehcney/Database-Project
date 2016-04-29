#include "Database.h"
#include "MultiMap.h"
#include "http.h"
#include <iostream> // needed for any I/O
#include <fstream>  // needed in addition to <iostream> for file I/O
#include <sstream>  // needed in addition to <iostream> for string stream I/O
#include <unordered_set>
using namespace std;

/////////////////////////////
// Database Implementations
/////////////////////////////

Database::Database()
{
	m_fieldIndex = nullptr;
}

Database::~Database()
{
	clearFieldIndex();
}

bool Database::specifySchema(const vector<FieldDescriptor>& schema)
{
	clearAll();

	for (int i = 0; i < schema.size(); ++i)
	{
		if (schema[i].index == it_indexed)
		{
			if (!m_fieldIndex)
				m_fieldIndex = new MultiMap*[schema.size()]();
			m_fieldIndex[i] = new MultiMap;
		}
	}
	if (!m_fieldIndex) // no indexable field
		return false;

	m_schema = schema;
	return true;
}

bool Database::addRow(const vector<string>& rowOfData)
{
	if (m_schema.empty() || m_schema.size() != rowOfData.size())
		return false;

	m_rows.push_back(rowOfData);
	for (int i = 0; i < rowOfData.size(); ++i)
		if (m_schema[i].index == it_indexed) // i-th field of rowOfData is indexed
			m_fieldIndex[i]->insert(rowOfData[i], m_rows.size() - 1);

	return true;
}

bool Database::loadFromURL(string url)
{
	string page;
	if (!HTTP().get(url, page))
		return false;

	// Check 1st line for proper schema
	istringstream iss(page);
	vector<FieldDescriptor> schema;
	string line, word;
	getline(iss, line);
	istringstream iss1(line);
	while (getline(iss1, word, ',')) // splitting by commas
	{
		FieldDescriptor temp;
		if (word.back() == '*')
		{
			temp.index = it_indexed;
			word.pop_back();
		}
		else
			temp.index = it_none;
		temp.name = word;
		schema.push_back(temp);
	}
	if (!specifySchema(schema)) // no indexed fields
		return false;

	// Process the rest of the lines
	vector<string> row;
	while (getline(iss, line))
	{
		row.clear();
		istringstream issLine(line);
		while (getline(issLine, word, ','))
			row.push_back(word);
		if (row.size() != m_schema.size()) // num data fields in row != schema
			return false;
		addRow(row); // add current row to m_rows (and m_fieldIndex if needed)
	}

	return true;
}

bool Database::loadFromFile(string filename)
{
	ifstream inf(filename);
	if (!inf)
		return false;

	// Check 1st line for proper schema
	vector<FieldDescriptor> schema;
	string line, word;
	getline(inf, line);
	istringstream iss1(line);
	while (getline(iss1, word, ',')) // splitting by commas
	{
		FieldDescriptor temp;
		if (word.back() == '*')
		{
			temp.index = it_indexed;
			word.pop_back();
		}
		else
			temp.index = it_none;
		temp.name = word;
		schema.push_back(temp);
	}
	if (!specifySchema(schema)) // no indexed fields
		return false;
	
	// Process the rest of the lines
	vector<string> row;
	while (getline(inf, line))
	{
		row.clear();
		istringstream issLine(line);
		while (getline(issLine, word, ','))
			row.push_back(word);
		if (row.size() != m_schema.size()) // num data fields in row != schema
			return false;
		addRow(row); // add current row to m_rows (and m_fieldIndex if needed)
	}

	return true;
}

int Database::getNumRows() const
{
	return m_rows.size();
}

bool Database::getRow(int rowNum, vector<string>& row) const
{
	if (rowNum < 0 || rowNum > getNumRows())
		return false;

	row = m_rows[rowNum];
	return true;
}

int Database::search(const vector<SearchCriterion>& searchCriteria,
	const vector<SortCriterion>& sortCriteria, vector<int>& results)
{
	results.clear();
	if (searchCriteria.empty()) // no search criteria
		return ERROR_RESULT;

	// Search
	//	The result of our query is the intersection of all search criteria. To do this, 
	//	for each search criteria, we plug our results into the unordered_set cur_query.
	//	For each subsequent search criteria, we copy cur_query into prev_query and clear
	//	cur_query, then check if the new value exists within prev_query: if it does, we 
	//	have an intersection, and we enter the value into cur_query.
	unordered_set<int> cur_query, prev_query;
	for (int i = 0; i < searchCriteria.size(); ++i)
	{
		if (searchCriteria[i].minValue == "" && searchCriteria[i].maxValue == "")
			return ERROR_RESULT; // lacks both min AND max values

		// Find fieldName within m_schema
		prev_query = cur_query;
		cur_query.clear();
		int j = 0;
		for (; j < m_schema.size(); ++j)
		{
			if (searchCriteria[i].fieldName == m_schema[j].name)
			{
				if (m_schema[j].index == it_none) // field name is non-indexed
					return ERROR_RESULT;

				// Find all rows between minValue and maxValue, inclusive.
				// If minValue or maxValue is empty, min and max iterators will be set to the
				// smallest and largest nodes, respectively.
				MultiMap::Iterator min = m_fieldIndex[j]->findEqualOrSuccessor(searchCriteria[i].minValue),
					max = m_fieldIndex[j]->findEqualOrPredecessor(searchCriteria[i].maxValue);
				for (; min.valid(); min.next())
				{
					bool add = true;
					if (i > 0) // only check the previous query if this is not our first
					{
						unordered_set<int>::const_iterator found = prev_query.find(min.getValue());
						if (found == prev_query.end()) // not found, so no intersection
							add = false;
					}
					if (add)
						cur_query.insert(min.getValue()); // inserts row # to current unordered_set
					if (min.getKey() == max.getKey() && min.getValue() == max.getValue())
						break;
				}
				break;
			}
		}
		if (j == m_schema.size()) // no fieldName match found
			return ERROR_RESULT;
	}
	
	// Sort
	for (auto it = cur_query.begin(); it != cur_query.end(); ++it)
		results.push_back(*it);
	if (!sortCriteria.empty())
		quicksort(results, 0, cur_query.size(), sortCriteria);

	return cur_query.size();
}


/////////////////////////////
// Database Helper Functions
/////////////////////////////

void Database::clearAll()
{
	clearSchema();
	clearRows();
	clearFieldIndex();
}

void Database::clearSchema()
{
	m_schema.clear();
}

void Database::clearRows()
{
	m_rows.clear();
}

void Database::clearFieldIndex()
{
	for (int i = 0; i < m_schema.size(); ++i)
		delete m_fieldIndex[i];
	delete m_fieldIndex;
	m_fieldIndex = nullptr;
}

int Database::compare(int a, int b, const vector<SortCriterion>& sortCriteria)
{
	for (int i = 0; i < sortCriteria.size(); ++i)
	{
		for (int j = 0; j < m_schema.size(); ++j)
		{
			if (m_schema[j].name == sortCriteria[i].fieldName)
			{
				if (sortCriteria[i].ordering == ot_ascending)
				{
					if (m_rows[a][j] > m_rows[b][j])
						return 1;
					if (m_rows[a][j] < m_rows[b][j])
						return -1;
				}
				else if (sortCriteria[i].ordering == ot_descending)
				{
					if (m_rows[a][j] < m_rows[b][j])
						return 1;
					if (m_rows[a][j] > m_rows[b][j])
						return -1;
				}
				break;
			}
		}
	}
	return 0;
}

void Database::swap(int& a, int& b)
{
	int t = a;
	a = b;
	b = t;
}

void Database::split(vector<int>& a, int start, int end, int pivot, int& firstEqualPivot, int& firstAfterPivot, 
	const vector<SortCriterion>& sortCriteria)
{
	// It will always be the case that just before evaluating the loop
	// condition:
	//  firstEqualPivot <= firstUnknown and firstUnknown <= firstAfterPivot
	//  Every element earlier than position firstEqualPivot is > pivot
	//  Every element from position firstEqualPivot to firstUnknown-1 is == pivot
	//  Every element from firstUnknown to firstAfterPivot-1 is not known yet
	//  Every element at position firstAfterPivot or later is < pivot

	firstEqualPivot = start;
	firstAfterPivot = end;
	int firstUnknown = start;
	while (firstUnknown < firstAfterPivot)
	{
		int test = compare(a[firstUnknown], pivot, sortCriteria);
		if (test > 0)
			swap(a[firstUnknown], a[--firstAfterPivot]); 
		else
		{
			if (test < 0)
				swap(a[firstEqualPivot++], a[firstUnknown]);
			firstUnknown++;
		}
	}
}

void Database::quicksort(vector<int>& a, int start, int end, const vector<SortCriterion>& sortCriteria)
{
	if (end - start <= 1)
		return;

	int firstEqualPivot, firstAfterPivot;
	split(a, start, end, a[start], firstEqualPivot, firstAfterPivot, sortCriteria);
	quicksort(a, start, firstEqualPivot, sortCriteria);
	quicksort(a, firstAfterPivot, end, sortCriteria);
}
