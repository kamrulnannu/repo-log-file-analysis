#include <bits/stdc++.h>

using namespace std;

struct FileRepo
{
    string m_FilePath;
    string m_Id;

    FileRepo() = default;
    FileRepo(const string & r, const string & i):
        m_FilePath(r), m_Id(i)
    {}

    friend ostream & operator << (ostream & os, const FileRepo & f)
    {
        os <<'('<< f.m_FilePath <<','<<f.m_Id<<") ";
        return os;
    }

    size_t hash() const
    {
        size_t h1 = std::hash<string>()(m_FilePath);
        size_t h2 = std::hash<string>()(m_Id);

        return h1 ^ (h2<<1);
    }

    bool operator == (const FileRepo & f) const
    {
        return ((m_FilePath == f.m_FilePath) && (m_Id == f.m_Id));
    }
};

#if 0
// OK, see alternative below
struct FileRepo_Hash
{
    size_t operator()(const FileRepo & f) const
    {
        return f.hash();
    }
};
#endif
namespace std
{
    template <>
        struct hash<FileRepo>
        {
            size_t operator()(const FileRepo &f) const
            {
                return f.hash();
            }
        };
}

struct QueryResult
{
    int m_CommitId;
    int m_TimeStamp;

    QueryResult():
        m_CommitId(-1),
        m_TimeStamp(-1)
    {}

    QueryResult(int cId, int ts):
        m_CommitId (cId),
        m_TimeStamp(ts)
    {}

    bool operator < (const QueryResult & qs) const
    {
#if 0
        if ((m_CommitId < qs.m_CommitId) || // Sort by increasing commit_id
            ((m_CommitId == qs.m_CommitId) &&
             (m_TimeStamp < qs.m_TimeStamp))) // Sort by increasing time_stamp
            return true;

        /*
         * Other cases:
         * 1. m_CommitId > qs.m_CommitId
         * 2. m_CommitId == qs.m_CommitId &&
         *    m_TimeStamp >= qs.m_TimeStamp
         */

        return false;
#endif
        if ((m_TimeStamp < qs.m_TimeStamp) || // Sort by increasing time_stamp
            ((m_TimeStamp == qs.m_TimeStamp) &&
             (m_CommitId < qs.m_CommitId))) // Sort by increasing commit_id
            return true;

        /*
         * Other cases:
         * 1. m_TimeStamp > qs.m_TimeStamp
         * 2. m_TimeStamp == qs.m_TimeStamp &&
         *    m_CommitId >= qs.m_CommitId
         */
        return false;
    }
};

class LogEntry
{
    private:
        int m_CommitId;
        int m_TimeStamp;
        unordered_set<FileRepo> m_Repos;

    public:
       LogEntry() = default;

        void setCommitId(int cid)
        {
            m_CommitId = cid;
        }
        void setTimeStamp(int time)
        {
            m_TimeStamp = time;
        }
        bool AddRepo(const FileRepo & f)
        {
            if (m_Repos.count(f))
            {
                cout<<"Err:Duplicate repo: "<< f<<endl;
                return false;
            }
            //m_Repos.insert(f); OK
            m_Repos.emplace(f.m_FilePath, f.m_Id);
            return true;
        }

        bool isTimeStampInRange(int start, int end) const
        {
            return (m_TimeStamp >= start && m_TimeStamp <= end);
        }

        int getCommitId() const
        {
            return m_CommitId;
        }
        int & getCommitId()
        {
            return m_CommitId;
        }
        int & getTimeStamp()
        {
            return m_TimeStamp;
        }
        const unordered_set<FileRepo> & getRepos() const
        {
            return m_Repos;
        }

        operator QueryResult () const
        {
            /*
             * Convert LogEntry object to QueryResult object
             */
            return {m_CommitId, m_TimeStamp};
        }

        friend ostream & operator << (ostream & os, const LogEntry & le)
        {
            os <<le.m_CommitId<<'|'<<le.m_TimeStamp<<'|';
            for (auto & a : le.m_Repos)
                cout<< a;
            cout<<'|';

            return os;
        }
};

struct Query
{
    int m_StartTime;
    int m_EndTime;
    FileRepo m_FP;
};

int main()
{
    vector<LogEntry> logs;
    vector<Query> query;
    unordered_multimap<FileRepo, int> repo2Idx;

    int N;
    cin >> N;
    cin.ignore(); // eat/ignore new line
    string line;
    int LineNum = 1;
#if 0
    // OK
    enum LogFieldId
    {
        COMMIT_ID_TAG = 0,
        COMMIT_ID_TAG_VAL = 1,
        TIME_STAMP_TAG = 2,
        TIME_STAMP_TAG_VAL = 3,
        FILE_PATH = 4,
        FILE_REPO_ID = 5
    };

    for (auto i = 0; i < N; ++i)
    {
        if (getline(cin, line))
        {
            //cout<<"LINE:"<<line<<"|\n";

            istringstream ss(line);

            string token;
            LogEntry le;
            LogFieldId nxt = COMMIT_ID_TAG;
            FileRepo fp;
            int tagVal = 0;
            while(getline(ss, token, ' '))
            {
                /*
                 * A entry line:
                 * commit_id val timestamp val file_1 id_1 ... fine_n id_n
                 */
                switch (nxt)
                {
                    case COMMIT_ID_TAG:
                        if (token.compare("commit_id"))
                        {
                            cerr<<"Error_0: Unknown commit_id tag: "<< token
                                <<" at line: "<< i+1<<"\n";
                            return -1;
                        }
                        nxt = COMMIT_ID_TAG_VAL;
                        break;
                    case COMMIT_ID_TAG_VAL:
                        le.setCommitId(stoi(token));
                        nxt = TIME_STAMP_TAG;
                        break;
                    case TIME_STAMP_TAG:
                        if (token.compare("timestamp"))
                        {
                            cerr<<"Error_1: Unknown time stamp tag: "<< token
                                <<" at line: "<< i+1 << endl;
                            return -1;
                        }
                        nxt = TIME_STAMP_TAG_VAL;
                        break;
                    case TIME_STAMP_TAG_VAL:
                        le.setTimeStamp(stoi(token));
                        nxt = FILE_PATH;
                        break;
                    case FILE_PATH:
                        fp.m_FilePath = token;
                        nxt = FILE_REPO_ID;
                        tagVal++;
                        break;
                    case FILE_REPO_ID:
                        fp.m_Id = token;
                        if (le.AddRepo(fp))
                        {
                            //repo2Idx.insert({fp, logs.size()}); OK
                            repo2Idx.emplace(std::piecewise_construct,
                                            std::tuple<string, string>(fp.m_FilePath, fp.m_Id),
                                            std::tuple<int>(logs.size()));
                            nxt = FILE_PATH;
                            tagVal++;
                        }
                        else
                        {
                            cerr<<"Error_2: Duplicate file repo info ("<<fp<<") at line: "
                                <<i+1<<endl;
                            return -1;
                        }
                }
            }
            if (tagVal > 0 && (tagVal % 2) == 0)
            {
                logs.push_back(le);
            }
            else
            {
                cerr<<"Error_3: No repo Id for a file path ("<<fp<<") at line: "
                    <<i+1<<endl;
                return -1;
            }
        }
        else
        {
            cerr<<"Error_4: getline failed!"<<endl;
            return -1;
        }
    }
#endif

    while (getline(cin, line))
    {
        LineNum++;
        istringstream ss(line);
        LogEntry le;
        string s_comId;
        string s_TimeSt;

        if ((ss >> s_comId >> le.getCommitId()
                >> s_TimeSt >> le.getTimeStamp()) &&
            (s_comId.compare("commit_id") == 0) &&
            (s_TimeSt.compare("timestamp") == 0))
        {
            if (ss.eof())
            {
                cerr<<"Error_1: File path and id missing, Line: "<<LineNum<<endl;
                return -1;
            }

            // Read FilePath and Id pairs
            FileRepo fp;
            while (!ss.eof())
            {
                if (ss >> fp.m_FilePath >> fp.m_Id)
                {
                    if (le.AddRepo(fp))
                    {
                        repo2Idx.emplace(std::piecewise_construct,
                                        std::tuple<string, string>(fp.m_FilePath, fp.m_Id),
                                        std::tuple<int>(logs.size()));
                    }
                    else
                    {
                        cerr<<"Error_2: Duplicate File/Id at line: "<<LineNum<<endl;
                        return -1;
                    }
                }
                else
                {
                    // EOF encountered
                    cerr<<"Error_3: File Id missing, line: "<<LineNum<<endl;
                    return -1;
                }
            }
            logs.push_back(le);
        }
        else
        {
            // EOF encountered
            cerr<<"Error_0: Insifficient/unknown data fields at line: "<<LineNum<<endl;
            return -1;
        }
        if (N == logs.size())
            break;
    }
    if (N != logs.size())
    {
        cerr<<"Error_5: Mismatch log entry, Expected: "<< N
            <<", Found: "<<logs.size()<<endl;
        return -1;
    }
    /*
    for (auto & l : logs)
        cout<<l<<'\n';
    */
    getline(cin, line);
    int Q; // Number of query
#if 0
    // OK
    cin >> Q;
    cin.ignore();
#endif
    try
    {
        Q = stoi(line);
    }
    catch(std::invalid_argument & e)
    {
        cerr<<"Error_9: stoi : invalid argument, Line# "<<N+1
            <<", Line:"<<line<<endl;
        return -1;
    }
    if (Q <= 0)
    {
        cerr<<"Error_10: Invalid number of query at line: "<<N+1<<'\n';
        return -1;
    }
    LineNum++;
    vector<Query> qList;
    while (getline(cin, line))
    {
        /*
         * A query line:
         * start_timestamp end_timestamp File id
         */
        istringstream ss(line);
        Query q;
        LineNum++;
        if (ss >> q.m_StartTime
                >> q.m_EndTime
                >> q.m_FP.m_FilePath
                >> q.m_FP.m_Id)
        {
            qList.push_back(q);
        }
        else
        {
            cerr<<"Error_6: Insifficient number of data fields at query line: "<<LineNum<<endl;
            return -1;
        }
        if (!ss.eof())
        {
            cerr<<"Error_11: Too many number of data fields at query line: "<<LineNum<<endl;
            return -1;
        }

#if 0
        // OK
        enum QueryField
        {
            Q_START_TIME_STAMP = 0,
            Q_END_TIME_STAMP = 1,
            Q_FILE_PATH = 2,
            Q_REPO_ID = 3,

            Q_Cardinality
        };

        QueryField nxt = Q_START_TIME_STAMP;
        int NumField = 0;
        while(getline(ss, token, ' '))
        {
            switch(nxt)
            {
                case Q_START_TIME_STAMP:
                    q.m_StartTime = stoi(token);
                    nxt = Q_END_TIME_STAMP;
                    NumField++;
                    break;
                case Q_END_TIME_STAMP:
                    q.m_EndTime = stoi(token);
                    nxt = Q_FILE_PATH;
                    NumField++;
                    break;
                case Q_FILE_PATH:
                    q.m_FP.m_FilePath = token;
                    nxt = Q_REPO_ID;
                    NumField++;
                    break;
                case Q_REPO_ID:
                    q.m_FP.m_Id = token;
                    NumField++;
                    break;
            }
        }
        if (NumField != Q_Cardinality)
        {
            cout<<"NF: "<< NumField<<endl;
            cerr<<"Error_6: Incorrect Query at line: "<< LineNum+i<<endl;
            return -1;
        }
        qList.push_back(q);
#endif
    }

    if (Q != qList.size())
    {
        cerr<<"Error_7: Querry number mismatch\n";
        return -1;
    }
    if (!cin.eof())
    {
        cerr<<"Error_8: Too many queries\n";
        return -1;
    }
    // Process queries
    for (auto & q : qList)
    {
        if (repo2Idx.count(q.m_FP) == 0)
        {
            /*
             * This file path and Id does not exist in log entry.
             * Print a blank line in the output
             */
            cout<<'\n';
            continue;
        }

        set<QueryResult> qsList;

        auto range = repo2Idx.equal_range(q.m_FP);
        for (auto it = range.first; it != range.second; ++it)
        {
            int idx = it->second;
            LogEntry & entry = logs[idx];
            if (entry.isTimeStampInRange(q.m_StartTime, q.m_EndTime))
            {
                QueryResult qs = entry; // Convert LogEntry to QueryResult
                qsList.insert(qs);
            }
        }
        // Response to stdout
        for (auto & qs : qsList)
        {
            cout<<qs.m_CommitId<<' ';
        }
        cout<<'\n';
    }
    return 0;
}
