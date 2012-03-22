/*

    A very easy to use tool for extracting indivual JPG based images from MPO files.

    Created by Kyle Johnson 3/19/2012

 */
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
bool splitmpo(string argv);
/*function... might want it in some class?*/
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

int main(int argc, char** argv)
{
    vector<string> files = vector<string>();
vector<string> mpofiles = vector<string>();

    if(argv[1]==NULL){
        string temp;
        cout << "\nPlease input name of Directory of .MPO images to process into JPG Artifacts: ";
        cin >>argv[0];
    }
    if(argv[0]==NULL){
        argv[0]=".";
    }
    argv[1]=argv[0];
    cout << "Scanning directory to read: "<< argv[1]<<endl;

    getdir(argv[1],files);
    int found,found2;//caps
    for (unsigned int i = 0;i < files.size();i++) {

        found=files[i].find(".mpo");
        found2=files[i].find(".MPO");
        if ((found)>0){

            mpofiles.push_back(files[i]);
        }else if ((found2)>0){
            mpofiles.push_back(files[i]);
        }
    }
    //convert all mpos
     for (unsigned int i = 0;i < mpofiles.size();i++) {
        splitmpo(mpofiles[i].c_str());
        if(i+1==mpofiles.size()){
        cout<<"\n____________________________\n\nTotal Files Processed: "<< mpofiles.size() << endl;
        }
     }

    cout << "\nThanks for using MPO Extractor!\n";
    return 0;
}
string Int2Str(int number)
{
    if (number == 0)
        return "0";
    string temp="";
    string returnvalue="";
    while (number>0)
    {
        temp+=number%10+48;
        number/=10;
    }
    for (int i=0; i<temp.length(); i++)
        returnvalue+=temp[temp.length()-i-1];
    return returnvalue;
}

bool splitmpo(string argv)
{
//   char c;//pointer
    int views = 0;  // number of views (JPG components)
    long length;    // total length of file
    //size_t amount;  // amount read

    char* buffer;
    string filename;
    string filenamebase;
//
    filename  = argv;
    filenamebase = filename;//toupper(argv[]);

    if (!filename.find(".mpo"))
    {
        //ext = "\0";
        cout << "Error finding file (Check all spelling and capitlizations please) "<< filename<<"\n" ;
        return false;
    }



    ifstream f (filename.c_str(),  ios::in | ios::app | ios::binary);    //Open the actual File

    if (!(f.is_open()) )
    {
        cout << "Error opening file "<< filename<<"\n" ;
        return false;
    }
    // obtain file size:
    // get length of file:
    f.seekg (0, ios::end);
    length = f.tellg();
    f.seekg (0, ios::beg);

    // allocate memory:
    buffer = new char [length];
    // allocate memory to contain the whole file:
    if (buffer == NULL)
    {
        cout<<"Failed to allocate memory\n";
        return false;
    }
    else
    {
        cout<<"Allocated "<<(int)length<<" chars of memory.\n";
    }
    // read data as a block:
    f.read(buffer,length);

    f.close();
    // NOW find the individual JPGs...
    char* view = buffer;
    char* last = NULL;
    string wnm,temp;
    cout << "Started at viewing"<<endl;
    while (view < buffer+length-4)
    {
        if (((char) view[0] % 255) == (char) 0xff)
        {
            if (((char) view[1] % 255) == (char) 0xd8)
            {
                if (((char) view[2] % 255) == (char) 0xff)
                {
                    if (((char) view[3] % 255) == (char) 0xe1)
                    {
                        cout << "View found at offset "<<view-buffer<<"\n";
                        views++;
                        if (last != NULL)
                        {
                            // copy out the previous view
                            temp.clear();
                            temp.append(filenamebase);
                            temp.append(Int2Str(views-1));
                            temp.append(".jpg");
                            ofstream w (temp.c_str(), ios::in| ios::out | ios::trunc|ios::binary);
                            //FILE* w = f.open(wnm, "wb");
                            w.write(last,view-last);
                            w.close();
                            cout<<"Created "<<wnm<<endl;
                        }
                        last = view;
                        view+=4;
                    }
                    else
                    {
                        view+=2;
                    }
                }
                else
                {
                    view+=3;
                }
            }
            else
            {
                view+=2;
            }
        }
        else
        {
            view+=1;
        }
    }

    if (views > 1)      // copy out the last view
    {
        temp.clear();
        temp.append(filenamebase);
        temp.append(Int2Str(views));
        temp.append(".jpg");
        ofstream w (temp.c_str(), ios::in| ios::out | ios::trunc|ios::binary);

        w.write(last,buffer+length-last);
        w.close();
        cout<< "Last Created "<<wnm<<endl;
    }
    else if (views == 0)
    {
        cout<< "No views found.\n";
    }
    cout<<"Stopped viewing file";
    delete[] buffer;
    return 0;

}
