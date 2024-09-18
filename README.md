A log line looks like:  

commit_id commit_id_value timestamp timestamp_value path_1 id_1 path_2 id_2 ... path_n id_n  

commit_id_value = non-negative integer, it uniq number.   
timestamp_value = non-negative integer  
path_i = non-space string, i = 1..n  
id_i = non-space string, i= 1..n  

Each of these path is described by its path and identifier (i.e.  
path_i id_i) and that is a function of the path and repository.  
Therefore, two commits for different repositories may contain the same  
file, but the file's id should be distinct and vice-versa.  

If a (path_i, Id_i) of a log line match with another log line then these two  
log lines (i.e. commit_ids) can be combined.  

For examples log entries in a log file:  
    commit_id 0 timestamp 1234 include/abc.h dc098a include/def.c rxpf67a  
    commit_id 1 timestamp 1234 include/ijk.h 345avc  
    commit_id 2 timestamp 2345 include.def.c rxpf67a src/pqr.c h967sw  

In above lines {include/def.c rxpf67a} exists in line 1 and 3 and hence  
these 2 lines represent one repository. Line 2 represent another  
repository.  

The code will read commit info (as described above) and queries info from  
standard input.  

The queries are laid out as follows:  
    start_timestamp end_timestamp path_i id_i  

Examples:  
    0 50000 quicksort.cpp f23ad5  
    0 5000 vector.h r567ad  
    0 40000 abc.h invalid_id  
    100 300 bogosort.cpp jf7634  
    ...  

The response of each query is the commit_id of each commit corresponding to  
the repository referenced by the query's {path_i id_i}, sorted by  
increasing timestamp followed by increasing commit_id and filtered by start  
and end timedstamp inclusive. Note that repositories should be determined  
based on all commits, not just the commit in the start and timestamp range.  

Output of the query should go to standard out.  
Ambiguous input should be reported along with line number.  

Input is provided via standard input. The first line is a integer L which  
represents number of log entries which follow.  
After the L log entries, there is another another entry integer Q which  
denotes number of queries which follow.  
