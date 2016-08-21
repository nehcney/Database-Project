// Each database is organized by specifying a schema. The schema tells the database what fields 
// are present in each record, and which of those fields should be indexed. When importing data,
// each field marked for indexing will be added to its respective BST multimap so that it can be 
// efficiently queried.

#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

class MultiMap;

class Database
{
public:
	enum IndexType { it_none, it_indexed };
	enum OrderingType { ot_ascending, ot_descending };

	struct FieldDescriptor
	{
		std::string name;
		IndexType index;
	};

	struct SearchCriterion
	{
		std::string fieldName;
		std::string minValue;
		std::string maxValue;
	};

	struct SortCriterion
	{
		std::string fieldName;
		OrderingType ordering;
	};

	static const int ERROR_RESULT = -1;

	Database();							// O(1)
	~Database();							// O(F)
	bool specifySchema(const std::vector<FieldDescriptor>& schema);	// O(F)
	bool addRow(const std::vector<std::string>& rowOfData);		// O(F log N)
	bool loadFromURL(std::string url);				// O(FN log N)
	bool loadFromFile(std::string filename);			// O(FN log N)
	int getNumRows() const;						// O(1)
	bool getRow(int rowNum, std::vector<std::string>& row) const;	// O(F)
	int search(const std::vector<SearchCriterion>& searchCriteria,	// O(CM log N + SR log R
		const std::vector<SortCriterion>& sortCriteria, 
		std::vector<int>& results);

private:
	std::vector<FieldDescriptor> m_schema;
	std::vector<std::vector<std::string> > m_rows;
	MultiMap** m_fieldIndex;

private:
	Database(const Database& other);
	Database& operator=(const Database& rhs);
	void clearAll();
	void clearSchema();
	void clearRows();
	void clearFieldIndex();
	int compare(int a, int b, const std::vector<SortCriterion>& sortCriteria);
	void swap(int& a, int& b);
	void split(std::vector<int>& a, int start, int end, int pivot, int& firstNotGreater, int& firstLess, 
		const std::vector<SortCriterion>& sortCriteria);
	void quicksort(std::vector<int>& a, int start, int end, const std::vector<SortCriterion>& sortCriteria);
};

#endif // DATABASE_H
