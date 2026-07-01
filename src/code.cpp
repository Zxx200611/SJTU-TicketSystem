// #include <gperftools/profiler.h>

#include<Train.hpp>
#include<Order.hpp>
#include<User.hpp>

int main()
{
    std::ios::sync_with_stdio(0);
    std::cin.tie(0),std::cout.tie(0);

    // std::cout<<"Init complete"<<std::endl;
    for(int _=1;;_++)
    {
        // std::cout<<"Processing command #"<<_<<std::endl;
        std::string s;
        std::getline(std::cin,s);

        static sjtu::map<std::string,std::string> M;
        M.clear();
        std::string hd=utils::parse(s,M);
        // std::cout<<"Parse done"<<std::endl;

        std::cout<<"["<<M["T"]<<"] ";
        // std::cout.flush();// !
        if(hd=="add_user")
        {
            int r=addUser(M["c"],M["u"],M["p"],M["n"],M["m"],utils::stringToInt(M["g"]));
            std::cout<<r-1<<'\n';
        }
        if(hd=="login")
        {
            int r=login(M["u"],M["p"]);
            std::cout<<r-1<<'\n';
        }
        if(hd=="logout")
        {
            int r=logout(M["u"]);
            std::cout<<r-1<<'\n';
        }
        if(hd=="query_profile")
        {
            int r=queryProfile(M["c"],M["u"]);
            if(r==0) std::cout<<"-1\n";
        }
        if(hd=="modify_profile")
        {
            int r=modifyProfile(M["c"],M["u"],M["p"],M["n"],M["m"]
                               ,M.count("g")?utils::stringToInt(M["g"]):-1);
            if(r==0) std::cout<<"-1\n";
        }
        if(hd=="add_train")
        {
            // std::cout<<"stations : "<<M["s"]<<std::endl;
            int n=utils::stringToInt(M["n"]);
            // std::cout<<(n)<<std::endl;
            // std::cout<<utils::stringToTime(M["x"])<<std::endl;
            int r=addTrain(M["i"],n,utils::stringToInt(M["m"]),M["s"],M["p"]
                          ,utils::stringToTime(M["x"]),M["t"],M["o"],M["d"],M["y"][0]);
                          
            // std::cout<<utils::stringToInt(M["n"])<<std::endl;
            std::cout<<r-1<<'\n';
        }
        if(hd=="delete_train")
        {
            int r=deleteTrain(M["i"]);
            std::cout<<r-1<<'\n';
        }
        if(hd=="release_train")
        {
            int r=releaseTrain(M["i"]);
            std::cout<<r-1<<'\n';
        }
        if(hd=="query_train")
        {
            int r=queryTrain(M["i"],utils::stringToDate(M["d"]));
            if(r==0) std::cout<<"-1\n";
        }
        if(hd=="query_ticket")
        {
            queryTicket(M["s"],M["t"],utils::stringToDate(M["d"]),M["p"]);
        }
        if(hd=="query_transfer")
        {
            queryTransfer(M["s"],M["t"],utils::stringToDate(M["d"]),M["p"]);
        }
        if(hd=="buy_ticket")
        {
            buyTicket(M["u"],M["i"],M["f"],M["t"],utils::stringToDate(M["d"])
                     ,utils::stringToInt(M["n"]),M["q"]=="true",utils::stringToInt(M["T"]));
        }
        if(hd=="query_order")
        {
            queryOrder(M["u"]);
        }
        if(hd=="refund_ticket")
        {
            refundTicket(M["u"],M.count("n")?utils::stringToInt(M["n"]):1);
        }
        if(hd=="clean")
        {
            login_list.clear();
            Users.clear();
            Trains.clear(),Arriv.clear(),Depar.clear();
            Orders.clear(),Pend.clear();
            std::cout<<"0\n";
        }
        if(hd=="exit")
        {
            // for(auto pr:time_used) std::cerr<<"Func "<<pr.first<<" uses "<<pr.second<<" ms"<<std::endl;
            // if(QT_count) std::cerr<<"Avr s_size = "<<S_size/QT_count<<", avr t_size = "<<T_size/QT_count<<std::endl;

            std::cout<<"bye"<<std::endl;
            return 0;
        }
        // std::cout<<"Done"<<std::endl;
        // Users.debugPrint();
    }
}