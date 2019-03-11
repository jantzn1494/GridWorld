
#ifndef _GRID_WORLD_H
#define _GRID_WORLD_H

#include <vector>
#include <iostream>

using std::vector;


class GridWorld {

  private:

        // struct for doubly-linked list nodes
        struct Node
        {
            int data;
            Node *next;
            Node *prev;

        };
        
        //class for lists
        class List{
            private:

                //List data members
                Node *front;
                Node *back;
                int size;

            public:

                //constructor
                List(){
                    front = nullptr;
                    back = nullptr;
                    size = 0;
                }

                //destructor
                ~List(){
                    clear();
                }
                
                //makes calling list empty
                void clear() {
                    Node *p = front;
                    Node *pnext;

                    while (p != nullptr)
                    {
                      pnext = p->next;
                      delete p;
                      p = pnext;
                    }
                    front = back = nullptr;
                    size = 0;
                }

                //returns copy of front
                Node * getFront(){
                    Node *p = front;
                    return p;
                }

                //returns size of calling list
                int length() const{
                    return size;
                }
                
                //appends node to end of list
                void append(Node * n){
                    if(this->front == nullptr){ //list is empty
                        front = n;
                        back = n;
                        n->prev = nullptr;
                        n->next = nullptr;
                        size++;
                    }
                    else{                                 //list is not empty
                        back->next = n;
                        n->prev = back;
                        n->next = nullptr;
                        back = n;
                        size++;
                    }
                }
                
                //deletes a node within a list
                void delNode(Node *n){

                    if(n == front){
                        front = n->next;
                        n->prev = nullptr;
                        n->next = nullptr;

                        
                    }
                    else if(n == back){
                        back = n->prev;
                        back->next = nullptr;
                        n->prev = nullptr;
                        n->next = nullptr;
                        
                    }
                    else{
                        n->prev->next = n->next;
                        n->next->prev = n->prev;
                        n->next = nullptr;
                        n->prev = nullptr;
                    }
                    size--;
                }

                //clones a list into a vector and returns a pointer to that vector
                std::vector<int> * clone()const
                {
                    vector<int> *clone = new vector<int>;
                    Node *p;
                    p = front;
                    while(p != nullptr){
                        clone->push_back(p->data);
                        p = p->next;
                    }
                    return clone;
                }
        };


        struct Person{
            //members
            int id;
            int row;
            int col;
            bool alive;
            Node *p;

            //constructor            
            Person(int id, int row, int col){
                this->id = id;
                this->row = row;
                this->col = col;
                alive = true;
                p = new Node;
                p->data = id;
            }
        };

        struct District{
            int row;
            int col;
            List members = List();

            District(int row, int col){
                this->row = row;
                this->col = col;
            }
        };



        //   etc.
    
        int pop; //population of GridWorld
        int nRows;    //number of rows for GridWorld
        int nCols;      //number of columns for GridWorld
        int idCounter;  //number of total ID's, in use or not in use

        vector<int> deadPool;     //ID's not in use, front number is oldest ID
        vector<Person> people; //all people, either alive or dead
        vector <District> world; //vector of districts
        
    

  public:
        /**
        * constructor:  initializes a "world" with nrows and
        *    ncols (nrows*ncols districtcs) in which all 
        *    districtricts are empty (a wasteland!).
        */
        GridWorld(unsigned nrows, unsigned ncols)   {
            this->pop = 0;                                //nobody in GridWorld
            this->nRows = nrows;                              //assigns nRows
            this->nCols = ncols;                                  //assigns nCols
            this->idCounter = 0;                                 //no ID's being used
            for(int i = 0; i < nRows; i++){    //creates every district
                for(int j = 0; j < nCols; j++){
                    this->world.push_back( District(i, j));
                }
            }
        }
        /*Destructor*/
        ~GridWorld(){
            pop = 0;
            nRows = 0;
            nCols = 0;
            idCounter = 0;
            people.erase(people.begin(), people.end());
            deadPool.erase(deadPool.begin(), deadPool.end());
            world.erase(world.begin(), world.end());
        }

        /*
         * function: birth
         * description:  if row/col is valid, a new person is created
         *   with an ID according to rules in handout.  New person is
         *   placed in district (row, col)
         *
         * return:  indicates success/failure
         */
        bool birth(int row, int col, int &id){
            if((row < nRows) && (col < nCols)){   //district (row, col) exists
                if(deadPool.empty()){                               //currently no unused IDs
                    Person *p;
                    p = new Person(idCounter, row, col); //new person
                    this->people.push_back(*p);  //added new person to people vector
                    id = idCounter;                 //used new ID
                    world.at((row * nCols) + col).members.append(people.at(id).p); //add node to member list of new district
                    idCounter++;                    //id has been used
                    pop++;                 //population increase
                    return true;
                }
                else{   //unused IDs available
                    id = deadPool.front(); //use oldest unused ID
                    deadPool.erase(deadPool.begin());       //remove it from deadPool
                    this->people.at(id).row = row;    //update row in person ID
                    this->people.at(id).col = col;        //update col in person ID
                    this->people.at(id).alive = true;// update alive in person ID
                    world.at((row * nCols) + col).members.append(people.at(id).p); //add node to member list of new district
                    pop++;                 //population increase
                    return true;                       
                }
            }
            else{
                return false;
            }
        }

        /*
         * function: death 
         * description:  if given person is alive, person is killed and
         *   data structures updated to reflect this change.
         *
         * return:  indicates success/failure
         */
        bool death(int personID){
            if((personID <= people.size()) && (people.at(personID).alive == true)){ //if person exists and is alive
                people.at(personID).alive = false;                                                                                           //person is now dead
                int row = people.at(personID).row;                                                                                         //assign row
                int col = people.at(personID).col;                                                                                             //assign col
                world.at((row * nCols) + col).members.delNode(people.at(personID).p);     //removes person node from  district member list
                deadPool.push_back(personID);
                pop--;
                return true;
            }
            else{
                return false;
            } 
       }

        /*
         * function: whereis
         * description:  if given person is alive, his/her current residence
         *   is reported via reference parameters row and col.
         *
         * return:  indicates success/failure
         */
        bool whereis(int id, int &row, int &col)const{
            if((id <= people.size()) && (people.at(id).alive == true)){
                row = people.at(id).row;
                col =  people.at(id).col;
                return true; 
            }
            else{
              return false;
            }
        }

        /*
         * function: move
         * description:  if given person is alive, and specified target-row
         *   and column are valid, person is moved to specified district and
         *   data structures updated accordingly.
         *
         * return:  indicates success/failure
         *
         * comment/note:  the specified person becomes the 'newest' member
         *   of target district (least seniority) --  see requirements of members().
         */
        bool move(int id, int targetRow, int targetCol){
            if((id <= people.size()) && (people.at(id).alive == true)){ //if person exists and is alive
                int oldRow = people.at(id).row;                                                   //saved old row stored in person
                int oldCol = people.at(id).col;                                                       //saved old col stored in person
                people.at(id).row = targetRow;                                                   //updates row
                people.at(id).col = targetCol;                                                       //updates col

                world.at((oldRow * nCols) + oldCol).members.delNode(people.at(id).p);     //removes person node from old district member list
                world.at((targetRow * nCols) + targetCol).members.append(people.at(id).p); //appends node to new district member list
                return true;
            }
            else{   //person doesn't exist or is dead
                return false;
            }
        }

        std::vector<int> * members(int row, int col)const{
            if((row < nRows) && (col < nCols)){
                return world.at((row * nCols) + col).members.clone();
            }
        }

        /*
         * function: population
         * description:  returns the current (living) population of the world.
         */
        int population()const{
          return this->pop;
        }

        /*
         * function: population(int,int)
         * description:  returns the current (living) population of specified
         *   district.  If district does not exist, zero is returned
         */
        int population(int row, int col)const{
            return this->world.at((row * nCols) + col).members.length();
        }

        /*
         * function: num_rows
         * description:  returns number of rows in world
         */
        int num_rows()const {
          return nRows;
        }

        /*
         * function: num_cols
         * description:  returns number of columns in world
         */
        int num_cols()const {
          return nCols;
        }



};

#endif
