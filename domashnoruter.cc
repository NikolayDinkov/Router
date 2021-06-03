#include <iostream>
#include <cstring>
#include <vector>
#include <list>
#include <fstream>

using namespace std;

class Package
{
    const string content;
    const int content_length;
    const string IP_coming;
    const string IP_to;
    const int currpack_number;

    static int packages_made;
public:
    int validate()
    {
        if(this->content_length == content.length())
        {
            return 1;
        }
        return 0;
    }
    Package(const string IP_coming, const string IP_to, const string content):
    content(content),
    content_length(content.length()),
    currpack_number(packages_made + 1),
    IP_coming(IP_coming),
    IP_to(IP_to)
    {
        packages_made++;
        if(IP_to == "0.0.0.0" || IP_to == "127.0.0.0" || IP_coming == "0.0.0.0" || IP_coming == "127.0.0.0")
        {
            throw "Not valueble IP!";
        }
        if(content.length() == 0)
        {
            throw "No content!";
        }
        cout << "(Package constructor): Package Num." << this->currpack_number << " is ready!" << endl;
    }
    string get_IP_to() const
    {
        return this->IP_to;
    }
    string get_IP_coming() const
    {
        return this->IP_coming;
    }
    int get_content_length() const
    {
        return this->content_length;
    }
    ~Package()
    {
        cout << "(Package destructor): I was here" << endl;
    }
};

int Package::packages_made = 0;

class Known_route
{
    string IP;
    int index;
    int sent_packs;
public:
    Known_route(string IP, int index)
    {
        this->IP = IP;
        this->index = index;
        this->sent_packs = 0;
    }
    string get_IP()
    {
        return this->IP;
    }
    int get_sent_packs()
    {
        return this->sent_packs;
    }
    void set_sent_packs(int sent_packsNew)
    {
        this->sent_packs = sent_packsNew;
    }
    int get_index()
    {
        return this->index;
    }
    bool operator < (const Known_route& second) const
    {
        return this->sent_packs < second.sent_packs;
    }
    bool operator > (const Known_route& second) const
    {
        return this->sent_packs > second.sent_packs;
    }
};
bool compare_ptrs_pre(Known_route* first, Known_route* second)
{
    return *first > *second;
}
class Router
{
    string router_name;
    string IP_router;
    vector<Router*> routers;
    list<Known_route*> routing_table;
    static const int max_routes;
    static const int max_hops;
    int successful_sendings;
public:
    Router(string name, string IP):
    router_name(name),
    IP_router(IP),
    successful_sendings(0)
    {
        cout << "(Router constructor): Hi, there is a new router." << endl;
    }
    static int get_max_hopes()
    {
        return max_hops;
    }
    string get_router_name()
    {
        return this->router_name;
    }
    string get_IP_router()
    {
        return this->IP_router;
    }
    void sorting_on_10()
    {
        if(this->successful_sendings % 10 == 0 && this->successful_sendings != 0)
        {
            cout << "(System): Sorting..." << endl;
            this->routing_table.sort(compare_ptrs_pre);
        }
    }
    void print()
    {
        cout << "Name: " <<this->router_name << " IP: " << this->IP_router << endl;
    }
    void add_router(const Router& router)
    {
        if(router.IP_router == "0.0.0.0" || router.IP_router == "127.0.0.0")
        {
            throw "Wrong IP!";
        }
        routers.push_back((Router*)&router);
    }
    int query_route(const string address, const int hop_count)
    {
        if(address == this->IP_router)
        {
            return 1;
        }
        if(hop_count == 0)
        {
            return 0;
        }
        for(list<Known_route*>::iterator it = this->routing_table.begin(); it != this->routing_table.end(); it++)
        {
            if((*it)->get_IP() == address)
            {
                return 1;
            }
        }
        for(vector<Router*>::iterator it = this->routers.begin(); it != this->routers.end(); it++) 
        {
            if((*it)->query_route(address, hop_count - 1))
            {
                Known_route* oneTimeOnly = new Known_route(address, it - this->routers.begin());
                if(this->routing_table.size() == max_routes)
                {
                    delete this->routing_table.back();
                    this->routing_table.back() = oneTimeOnly;
                    return 1;
                }
                this->routing_table.push_back(oneTimeOnly);
                return 1;
            }
        }
        return 0;
    }
    void send_package(const Package& package)
    {
        if(package.get_content_length() == 0)
        {
            throw "Empty package!";
        }
        if(package.get_IP_to() == "0.0.0.0" || package.get_IP_to() == "127.0.0.0" || 
        package.get_IP_coming() == "0.0.0.0" || package.get_IP_coming() == "127.0.0.0")
        {
            throw "Package IP is wrong!";
        }
        if(package.get_IP_to() == this->IP_router)
        {
            cout<< "(System): Package reached its destination!" << endl;
            this->successful_sendings++;
            this->sorting_on_10();
            return;
        }
        for(list<Known_route*>::iterator it = this->routing_table.begin(); it != this->routing_table.end(); it++)
        {
            if((*it)->get_IP() == package.get_IP_to())
            {
                (*it)->set_sent_packs((*it)->get_sent_packs() + 1);
                cout << "(Package): Still need to travel, I am going to my friend in the vector." << endl;
                this->routers.at((*it)->get_index())->send_package(package);
                return;
            }
        }
        if(this->query_route(package.get_IP_to(), Router::max_hops) == 1)
        {
            cout << "(Package): We did the search!" << endl;
            for(list<Known_route*>::iterator it = this->routing_table.begin(); it != this->routing_table.end(); it++)
            {
                if((*it)->get_IP() == package.get_IP_to())
                {
                    (*it)->set_sent_packs((*it)->get_sent_packs() + 1);
                    cout << "(Package): After search: Still need to travel, I am going to my friend in the vector." << endl;
                    this->routers.at((*it)->get_index())->send_package(package);
                    return;
                }
            }
        }else{
            cout << "(Package): There is no home for me. BYE!" << endl << "(System): Package is destroyed!" << endl;
            return;
        }    
    }
    ~Router()
    {
        cout << "(Router destructor): I did the job!" << endl;
        for(list<Known_route*>::iterator it = this->routing_table.begin(); it != this->routing_table.end(); it++)
        {
            delete (*it);
        }
    }
};

const int Router::max_routes = 20;
const int Router::max_hops = 10;

int main()
{
    /*Package a = Package("1.5", "1.1", "That is me open the door.");
    Package b = Package("1", "2", "Now!");
    cout << "a - validate: " << a.validate() << endl;
    Router r1 = Router("Dinkovi", "1.1");
    Router r2 = Router("Mtel", "3");
    r1.add_router(r2);
    cout << r1.query_route("1.", Router::get_max_hopes()) << endl;
    r1.send_package(a);
    //try
    {
        Package p1("1.1.1.1", "4.4.4.4", "Hello world");
        Package p2("1.1.1.1", "6.6.6.6", "Hel world");
        Package p3("1.1.1.1", "5.5.5.5", "Hl world");
        Router r1("ruter1", "3.3.3.3");
        Router r2("ruter2", "2.2.2.2");
        Router r3("ruter3", "6.6.6.6");
        Router r4("ruter4", "5.5.5.5");
        Router r5("ruter5", "4.4.4.4");

        r1.add_router(r2);
        r2.add_router(r3);
        r3.add_router(r4);
        r4.add_router(r5);

        r1.query_route("4.4.4.4", Router::get_max_hopes());

        r1.Routing_Table.push_back(new Path("1.1.1.1",0));
        r1.Routing_Table.push_back(new Path("2.2.2.2",1));
        r1.Routing_Table.push_back(new Path("3.3.3.3",2));
        r1.Routing_Table.push_back(new Path("4.4.4.4",3));
        r1.Routing_Table.push_back(new Path("5.5.5.5",4));
        r1.send_package(p1);
        r1.send_package(p2);
        r1.send_package(p3);*/
    try
    {    
        ifstream routers("routers.txt");

        vector<Router*> routers_from_file;
        string temp_name;
        string temp_IP;
        while(!routers.eof())
        {
            routers >> temp_name >> temp_IP;
            routers_from_file.push_back(new Router(temp_name, temp_IP));
        }

        routers.close();

        ifstream networks("network.txt");

        while(!networks.eof())
        {
            string name1;
            string name2;
            networks >> name1 >> name2;
            int index1 = -1;
            int index2 = -1;
            for(int i = 0; i < routers_from_file.size(); i++)
            {
                if(routers_from_file.at(i)->get_router_name() == name1)
                {
                    index1 = i;
                }
                if(routers_from_file.at(i)->get_router_name() == name2)
                {
                    index2 = i;
                }
                if(index1 != -1 && index2 != -1)
                {
                    routers_from_file.at(index1)->add_router(*routers_from_file.at(index2));
                    routers_from_file.at(index2)->add_router(*routers_from_file.at(index1));
                    index1 = -1;
                    index1 = -1;
                }
            }
        }
        networks.close();

        ifstream packages("packages.txt");
        int a = 0;
        while(!packages.eof())
        {
            string temp_IP_coming;
            string temp_IP_to;
            string temp_content;
            packages >> temp_IP_coming >> temp_IP_to;
            getline(packages, temp_content);
            temp_content.erase(temp_content.begin());
            temp_content.erase(temp_content.begin());
            temp_content.erase(temp_content.end() - 1);
            Package *read_pack = new Package(temp_IP_coming, temp_IP_to, temp_content);
            for(auto it = routers_from_file.begin(); it < routers_from_file.end(); it++)
            {
                if((*it)->get_IP_router() == read_pack->get_IP_coming())
                {
                    (*it)->send_package(*read_pack);
                }
            }
            delete read_pack;
        }

        packages.close();

        for(int i = 0; i < routers_from_file.size(); i++)
        {
            delete routers_from_file.at(i);
        }
    }catch(const char* err){
        cout << err << endl;
    }

    return 0;
}

