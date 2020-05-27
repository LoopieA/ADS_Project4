#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// BST node
struct node
{
    string key;
    string name;
    string version;
    int file_position;
    int quantity;
    float price;
    struct node* left, * right;
};

class SoftwareStore {
private:
public:
    struct node* newNode(string item, string name, string version, int file_position, int quantity, float price) {
        struct node* temp = new node;
        temp->key = item;
        temp->name = name;
        temp->version = version;
        temp->file_position = file_position;
        temp->quantity = quantity;
        temp->price = price;
        temp->left = temp->right = NULL;
        return temp;
    }

    // Function doing inorder traversal of BST
    void inorder(struct node* root) {
        if (root != NULL) {
            inorder(root->left);
            cout << root->key << '\n';
            cout << "    Quantity:" << root->quantity << '\n';
            cout << "    Price:" << root->price << '\n';
            cout << "    FilePos:" << root->file_position << '\n';
            inorder(root->right);
        }
    }

    //function to dump changes to file
    void store_to_file(ofstream& out, struct node* root, int file_pos) {
        int currLine = 1;
        node* curr;
        while (currLine <= file_pos)
        {
            curr = search_by_pos(root, currLine);
            if (curr)
                out << curr->name + "," + curr->version + "," + to_string(curr->quantity) + "," + to_string(curr->price) + ",\n";
            currLine++;
        }
    }

    // Function to insertNode a new node with given key in BST
    struct node* insertNode(struct node* node, string key, string name, string version, int file_position, int quantity, float price) {
        /* If the tree is empty, return a new node */
        if (node == NULL) return newNode(key, name, version, file_position, quantity, price);

        // Otherwise, recur down the tree
        if (key < node->key)
            node->left = insertNode(node->left, key, name, version, file_position, quantity, price);
        else if (key > node->key)
            node->right = insertNode(node->right, key, name, version, file_position, quantity, price);
        // return the node pointer
        return node;
    }

    struct node* search(struct node* root, string key) {
        // root is null or key is present at root 
        if (root == NULL || root->key == key)
            return root;

        // key is greater than root's key 
        if (root->key < key)
            return search(root->right, key);

        // key is smaller than root's key 
        return search(root->left, key);
    }

    struct node* search_by_pos(node* root, int file_pos) {
        if (root == NULL || root->file_position == file_pos)
            return root;

        node* res = search_by_pos(root->right, file_pos);

        if (!res)
            return search_by_pos(root->left, file_pos);
        else
            return res;
    }

    struct node* minValueNode(struct node* node) {
        struct node* current = node;

        // loop down to find the leftmost leaf
        while (current && current->left != NULL)
            current = current->left;

        return current;
    }

    struct node* deleteNode(struct node* root, string key) {
        // base case 
        if (root == NULL) return root;

        // If the key to be deleted is smaller than the root's key, then it lies in left subtree 
        if (key < root->key)
            root->left = deleteNode(root->left, key);

        // If the key to be deleted is greater than the root's key, then it lies in right subtree 
        else if (key > root->key)
            root->right = deleteNode(root->right, key);

        else
        {
            if (root->left == NULL)
            {
                struct node* temp = root->right;
                free(root);
                return temp;
            }
            else if (root->right == NULL)
            {
                struct node* temp = root->left;
                free(root);
                return temp;
            }

            // get the inorder successor (smallest in the right subtree) 
            struct node* temp = minValueNode(root->right);

            // copy the inorder successor's key to root
            root->key = temp->key;

            // delete the inorder successor 
            root->right = deleteNode(root->right, temp->key);
        }
        return root;
    }
};

bool is_empty_file(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}

void addNewPackage(SoftwareStore& softwarestore, node* root, int& file_pos) {
    string name_soft;
    string version_soft;
    int quantity_soft;
    float price_soft;
    cout << "Name of the software: "; cin.ignore(); getline(cin, name_soft); cout << '\n';
    cout << "Version of the software (format should be X.X): "; cin >> version_soft; cout << '\n';
    node* search_result = softwarestore.search(root, name_soft + " v" + version_soft);
    cout << "Quantity of the software: "; cin >> quantity_soft; cout << '\n';
    cout << "Price of the software (input 0 if you want to just update the software package without changing the price): "; cin >> price_soft; cout << '\n';
    if (search_result != NULL) {
        cout << "An existing record of " + search_result->key + " has been updated. \n";
        if (price_soft != 0)
            search_result->price = price_soft;
        search_result->quantity += quantity_soft;
    }
    else
    {
        softwarestore.insertNode(root, name_soft + " v" + version_soft, name_soft, version_soft, file_pos, quantity_soft, price_soft);
        file_pos++;
    }
}

void sellPackage(SoftwareStore& softwarestore, node* root)
{
    string software_name;
    cout << "Software package name (format: software_name vX.X): "; cin.ignore();  getline(cin, software_name);
    node* search_result = softwarestore.search(root, software_name);
    if (search_result != NULL) {
        search_result->quantity -= 1;
        if (search_result->quantity == 0) {
            root = softwarestore.deleteNode(root, search_result->key);
            cout << "Entry has now quantity 0.";
        }
        else
            cout << search_result->key << " was updated.\n";
    }
    else
        cout << software_name << " was not found in the store. Consider adding it by entering 2.\n";
}

int main()
{
    SoftwareStore* softwarestore = new SoftwareStore;
    ifstream file("store.txt");
    size_t pos = 0;
    string delimiter = ",", token;  //using , as a delimiter for the name of the software, version, quantity, price
    node* root = NULL;
    int file_pos = 1; //counter for the lines of the file

    //populate BST from file
    if (file.is_open()) {
        if (!is_empty_file(file))
        { 
            string line;  //dealing with the first line of the store
            getline(file, line);
            int token_i = 0;
            string result[4] = {};
            while ((pos = line.find(delimiter)) != std::string::npos) {  //delimiting line into tokens
                token = line.substr(0, pos);
                result[token_i] = token;
                line.erase(0, pos + delimiter.length());
                token_i++;
            }
            root = softwarestore->insertNode(root, result[0] + " v" + result[1], result[0], result[1], file_pos, stoi(result[2]), stoi(result[3]));
            file_pos++;
            //now taking care of the rest of the store text
            while (getline(file, line)) {
                int token_i = 0;
                string result[4] = {};
                while ((pos = line.find(delimiter)) != std::string::npos) {
                    token = line.substr(0, pos);
                    result[token_i] = token;
                    line.erase(0, pos + delimiter.length());
                    token_i++;
                }
                softwarestore->insertNode(root, result[0] + " v" + result[1], result[0], result[1], file_pos, stoi(result[2]), stoi(result[3])); //inserting the data into the BST
                file_pos++;
            }
            file.close();
        }
        else {
            cout << "The file is empty, please input a new software package. \n";
            string name_soft;
            string version_soft;
            int quantity_soft;
            float price_soft;
            cout << "Name of the software: "; cin.ignore(); getline(cin, name_soft); cout << '\n';
            cout << "Version of the software (format should be X.X): "; cin >> version_soft; cout << '\n';
            cout << "Quantity of the software: "; cin >> quantity_soft; cout << '\n';
            cout << "Price of the software: "; cin >> price_soft; cout << '\n';
            root = softwarestore->insertNode(root, name_soft + " v" + version_soft, name_soft, version_soft, file_pos, quantity_soft, price_soft);
            file_pos++;
            file.close();
        }
    }

    int choice;
    
    do {
        cout << "1 - Display software packages \n";
        cout << "2 - Add new software packages \n";
        cout << "3 - Sell software packages \n";
        cout << "0 - EXIT \n";
        cin >> choice;
        switch (choice)
        {
            case 1:
            {
                softwarestore->inorder(root);
                break;
            }
            case 2:
            {
                addNewPackage(*softwarestore, root, file_pos);
                break;
            }
            case 3:
            {
                sellPackage(*softwarestore, root);
                break;
            }
            case 0:
            {
                cout << "Thank you for using our services!" << "\n \n";
                ofstream out("store.txt");
                softwarestore->store_to_file(out, root, file_pos);
                out.close();
                break;
            }
            default:
                cout << "Invalid choice";
        }
    } while (choice != 0);
    
    return 0;
}
