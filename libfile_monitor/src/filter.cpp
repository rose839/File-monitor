#include <fstream>
#include <stdexcept>
#include <regex>
#include "filter.h"

using namespace std;

namespace fm {

	static bool is_unescaped_space(string& filter, long i) {
	    if (filter[i] != ' ') return false;

	    unsigned int backslashes = 0;

	    while (--i >= 0 && filter[i] == '\\') ++backslashes;

	    return (backslashes % 2 == 0);
  	}

	static bool parse_filter(string filter, Monitor_filter& filter_object, void (*err_handler)(string)) {

		/* skip empty strings */
		if (filter.length() == 0) return false;

		/* strip comments */
		if (filter[0] == '#') return false;

		/* Valid filters have the following structure:
		 *     type pattern
		 * where type may contains the following characters:
		 *   - '+' or '-', to indicate whether the filter is an inclusion or an exclusion filter.
		 *   - 'e', for an extended regular expression.
		 *   - 'i', for a case insensitive regular expression.
		 */
		regex filter_grammar("^([+-])([ei]*) (.+)$", regex_constants::extended);
		smatch fragments;

		if (!regex_match(filter, fragments, filter_grammar)) {
		  	handle_error(filter);
		  	return false;
		}

		/* reset the filter object to its default values */
		filter_object = {};
		filter_object.case_sensitive = true;

		/* name the fragments */
		string frag_type   = fragments[1].str();
		string frag_flag   = fragments[2].str();
		string frag_filter = fragments[3].str();

		/* build the filter */
		switch (frag_type[0]) {
			case '+':
		  		filter_object.type = fm_filter_type::filter_include;
		  		break;
			case '-':
		 		filter_object.type = fm_filter_type::filter_exclude;
		  		break;
			default:
		  		throw invalid_argument(string("Unknown filter type: ") + frag_type[0]);
		}

		/* parse the flags */
		for (char c : frag_flag) {
		    switch (c) {
		    	case 'e':
		      		filter_object.extended = true;
		      		break;
		    	case 'i':
		     	 	filter_object.case_sensitive = false;
		      		break;
		    	default:
		      		throw invalid_argument(string("Unknown flag: ") + c);
		    }
		}

		/* parse the filter */
		/* trim unescaped trailing spaces. */
		for (auto i = frag_filter.length() - 1; i > 0; --i) {
			if (is_unescaped_space(frag_filter, i)) {
				frag_filter.erase(i, 1);
			} else {
				break;
			}
		}

		/* if a single space is the only character left then the filter is invalid. */
		if (frag_filter.length() == 1 && frag_filter[0] == ' ') {
		  	handle_error(filter);
		  	return false;
		}

		/* Ignore empty lines */
		if (frag_filter.length() == 0) {
		  	handle_error(filter);
		  	return false;
		}

		filter_object.text = frag_filter;

		return true;
	} 
	
	std::vector<Monitor_filter> Monitor_filter::read_from_file(const std::string& path,
	                                                          void (*err_handler)(std::string)) {
		ifstream fobj(path);

		if (!fobj.is_open()) {
			throw invalid_argument(string("file not found: ") + path);
		}

		Monitor_filter filter;
		vector<Monitor_filter> filters;
		string line;

		while (getline(fobj, line)) {
			if (parse_filter(line, filter, err_handler)) {
				filters.push_back(filter);
			}
		}

		return filters;
	}

}
