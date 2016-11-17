// Jaroslav Jindrak
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <utility>

#define DEBUG_MESSAGES 0
#define RUN_TESTS 0

#if DEBUG_MESSAGES == 1
#define DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
#define DEBUG(msg)
#endif

#if RUN_TESTS == 1
#define TEST(msg, num) std::cout << "[TEST #" << num << "] " << msg << std::endl
#else
#define TEST(msg, num)
#endif

/**
 * Node template used in the test,
 * at the moment only contains key as
 * the value was not necessary for the task.
 */
template<typename T>
struct Node
{
	/**
	 * Key identifying this node.
	 */
	T key;

	/**
	 * Default constructor.
	 */
	Node() = default;

	/**
	 * Destructor.
	 */
	~Node() = default;

	/**
	 * Constructor.
	 * Param: Key of this node.
	 */
	Node(T k)
		: key{k}, parent{},
		  left{}, right{}
	{ /* DUMMY BODY */ }

	/**
	 * Pointer to the parent node.
	 */
	Node<T>* parent;
	
	/**
	 * Pointer to the left child node.
	 */
	Node<T>* left;
	
	/**
	 * Pointer to the right child node.
	 */
	Node<T>* right;
};

/**
 * Auxiliary namespace containing functions used
 * for better code readability.
 */
namespace utils
{
	/**
	 * Returns true if a given node is the left
	 * son of its parent.
	 */
	template<typename T>
	bool is_left_son(Node<T>* node)
	{
		return node->parent->left == node;
	}

	/**
	 * Returns true if a given node is a child of
	 * a node that is the left son of its parent
	 * node.
	 */
	template<typename T>
	bool is_son_of_left_son(Node<T>* node)
	{
		return node->parent->parent->left == node->parent;
	}

	/**
	 * Returns true if a given node is the right
	 * son of its parent.
	 */
	template<typename T>
	bool is_right_son(Node<T>* node)
	{
		return node->parent->right == node;
	}

	/**
	 * Returns true if a given node is a child of a
	 * node that is the right son of its parent node.
	 */
	template<typename T>
	bool is_son_of_right_son(Node<T>* node)
	{
		return node->parent->parent->right == node->parent;
	}

	/**
	 * Returns true if a given node is the left son of its
	 * parent node and that node is the left son of its
	 * parent node.
	 */
	template<typename T>
	bool is_left_son_of_left_son(Node<T>* node)
	{
		return is_left_son(node) && is_son_of_left_son(node);
	}

	/**
	 * Returns true if a given node is the right son of its
	 * parent node and that node is the right son of its
	 * parent node.
	 */
	template<typename T>
	bool is_right_son_of_right_son(Node<T>* node)
	{
		return is_right_son(node) && is_son_of_right_son(node);
	}

	/**
	 * Returns true if the father of a given node is the
	 * root of the splay tree.
	 */
	template<typename T>
	bool is_son_of_root(Node<T>* node)
	{
		return node->parent && !node->parent->parent;
	}

	/**
	 * Returns true if a given node is the right son of its
	 * parent node and that node is the left son of its
	 * parent node.
	 */
	template<typename T>
	bool is_right_son_of_left_son(Node<T>* node)
	{
		return is_right_son(node) && is_son_of_left_son(node);
	}

	/**
	 * Returns true if a given node is the left son of its
	 * parent node and that node is the right son of its
	 * parent node.
	 */
	template<typename T>
	bool is_left_son_of_right_son(Node<T>* node)
	{
		return is_left_son(node) && is_son_of_right_son(node);
	}

	/**
	 * Auxiliary comparer, simple operator overloading could've
	 * been used but I wanted to implement this more in the spirit
	 * of the STL.
	 */
	template<typename T>
	struct SplayComparator
	{
		/**
		 * Returns true if the key of a given node is less
		 * than a given key.
		 */
		bool operator()(const Node<T>& a, const T& key) const
		{
			return a.key < key;
		}
	};
}

/**
 * Class that represents a splay tree that contains keys of
 * a given type and uses a given splay policy.
 */
template<typename T, typename SplayPolicy, typename Comparator = utils::SplayComparator<T>>
class SplayTree
{
	friend bool test_3();
	public:
		/**
		 * Constructor.
		 */
		SplayTree() = default;

		/**
		 * Destructor.
		 */
		~SplayTree()
		{
			if(root_)
			{
				delete_(root_);
				delete root_;
			}
		}

		/**
		 * Inserts the given key into the splay tree
		 * if that key is not yet present in the tree.
		 */
		void insert(const T& key)
		{
			if(!root_)
			{
				root_ = new Node<T>{key};
				return;
			}

			auto closest = find_node_with_closest_key_(key);
			
			if(closest)
			{ // Otherwise the key is already present.
				SplayPolicy::splay(closest, &root_);

				if(closest->key == key)
					return; // Already present.

				auto tmp = new Node<T>{key};
				if(comparator_(*root_, key))
				{
					tmp->right = root_->right;
					root_->right = tmp;
					tmp->parent = root_;

					if(tmp->right)
						tmp->right->parent = tmp;
				}
				else
				{
					tmp->left = root_->left;
					root_->left = tmp;
					tmp->parent = root_;
				
					if(tmp->left)
						tmp->left->parent = tmp;
				}
			}
		}

		/**
		 * Returns true if this tree contains
		 * this key already.
		 */
		bool contains(const T& key)
		{
			auto found_key = find(key);
			return key == found_key;
		}

		/**
		 * Returns the key of a node that has the given key.
		 *
		 * ...
		 *
		 * I know, I know, this was just used for the test,
		 * normally it'd return the value.
		 */
		T find(const T& key)
		{
			static T NOT_FOUND{};
			auto closest = find_node_with_closest_key_(key);
			SplayPolicy::splay(closest, &root_);

			if(root_ && root_->key == key)
				return root_->key;
			else
				return NOT_FOUND;
		}

		/**
		 * Returns true if the tree is a valid binary
		 * search tree.
		 */
		bool validate() const
		{
			return validate_(root_);
		}

		/**
		 * Prints an auxiliary string representation of
		 * this tree.
		 */
		void print() const
		{
			DEBUG(print_(root_));
		}

		/**
		 * Returns the length of the last find traversal.
		 */
		std::size_t length_of_last_find() const
		{
			return find_length_;
		}

	private:
		/**
		 * Root node of the splay tree.
		 */
		Node<T>* root_;

		/**
		 * Comparator used to navigate the tree on
		 * insert/find.
		 */
		Comparator comparator_;

		/**
		 * Returns true if a given node and its two
		 * subtrees are valid binary search tree.
		 */
		bool validate_(Node<T>* node) const
		{
			if(!node)
				return true;

			if(node->left && node->left->key > node->key)
				return false;
			if(node->right && node->right->key < node->key)
				return false;
			
			return validate_(node->right) && validate_(node->left);
		}

		/**
		 * Finds the node whose key is the closest to a given
		 * key.
		 */
		Node<T>* find_node_with_closest_key_(const T& key)
		{
			find_length_ = std::size_t{};

			auto current_node = root_;
			auto prev_node = root_;

			while(current_node)
			{
				prev_node = current_node;
				if(current_node->key == key)
					return current_node;
				else if(comparator_(*current_node, key))
					current_node = current_node->right;
				else
					current_node = current_node->left;
				++find_length_;
			}

			return prev_node;
		}

		/**
		 * Prints a single node and its subtree.
		 */
		std::string print_(Node<T>* node) const
		{
			if(node)
			{
				/**
				 * And then God said "Let there be Lisp!", and he
				 * saw it good.
				 */
				return std::to_string(node->key) +
					   (node->left || node->right ? " " : "") + 
					   (node->left ? "L(" + print_(node->left) + ")" : "") +
					   (node->right ? "R(" + print_(node->right) + ")" : "");
			}
			else
				return "";
		}

		/**
		 * Deletes a single node and its subtree.
		 * (Deleting root_ effectively deallocates the tree.)
		 */
		void delete_(Node<T>* node)
		{
			if(!node)
				return;
			if(node->left)
			{
				delete_(node->left);
				delete node->left;
			}
			if(node->right)
			{
				delete_(node->right);
				delete node->right;
			}
		}

		/**
		 * Variable keeping track of the length of the last traversal.
		 */
		std::size_t find_length_;
};

/**
 * Auxiliary class implementing rotations on splay trees, this approach was
 * chosen to avoid unnecessary use of inheritance.
 * Note: Comments talking about movement, root and pivot, alpha, beta and gamma
 *       are all refering to this gif:
 *       https://upload.wikimedia.org/wikipedia/commons/3/31/Tree_rotation_animation_250x250.gif
 *       This is because, to this day, tree rotations confuse me.
 *       (I know, I know, shameful.)
 */
template<typename T>
struct SplayTreeRotator
{
	/**
	 * Performs a left rotation around the given node.
	 * Param: Root of the rotated subtree.
	 * Param: Root of the entire tree.
	 */
	static void rotate_left(Node<T>* node, Node<T>** tree_root)
	{
		if(!node || !tree_root || !(*tree_root))
			return;

		auto right = node->right; // Pivot.
		if(right)
		{
			// Beta horizontal movement.
			if(right->left)
				right->left->parent = node;
			node->right = right->left;

			// Swap of root and pivot.
			right->left = node;
			right->parent = node->parent;
		}

		// Finnish the swap of root and pivot,
		// i.e. update root or children of root's
		// parent.
		if(!node->parent)
			(*tree_root) = right;
		else if(utils::is_left_son(node))
			node->parent->left = right;
		else
			node->parent->right = right;
		node->parent = right;
	}

	/**
	 * Performs a right rotation around the given node.
	 * Param: Root of the rotated subtree.
	 * Param: Root of the entire tree.
	 */
	static void rotate_right(Node<T>* node, Node<T>** tree_root)
	{
		if(!node || !tree_root)
			return;
	
		auto left = node->left; // Pivot.
		if(left)
		{
			// Beta horizontal movement.
			if(left->right)
				left->right->parent = node;
			node->left = left->right;

			// Swap of root and pivot.
			left->right = node;
			left->parent = node->parent;
		}

		// Finnish the swap of root and pivot,
		// i.e. update root or children of root's
		// parent.
		if(!node->parent)
			(*tree_root) = left;
		else if(utils::is_left_son(node))
			node->parent->left = left;
		else
			node->parent->right = left;
		node->parent = left;
	}
};

/**
 * Standard implementation of the splay operation using the
 * zig, zigzig and zigzag step during propagation.
 */
template<typename T>
struct DoubleRotationSplayPolicy
{
	/**
	 * Propagates a given node to the top of the tree.
	 */
	static void splay(Node<T>* node, Node<T>** root)
	{
		if(!node || !root || !node->parent)
			return;

		while(node->parent)
		{
			if(utils::is_son_of_root(node))
			{ // Zig.
				if(utils::is_left_son(node))
					SplayTreeRotator<T>::rotate_right(node->parent, root);
				else
					SplayTreeRotator<T>::rotate_left(node->parent, root);
			}
			else if(utils::is_left_son_of_left_son(node))
			{ // Zig-zig.
				SplayTreeRotator<T>::rotate_right(node->parent->parent, root);
				SplayTreeRotator<T>::rotate_right(node->parent, root);
			}
			else if(utils::is_right_son_of_right_son(node))
			{ // Zig-zig 2: Zig-zig harder.
				SplayTreeRotator<T>::rotate_left(node->parent->parent, root);
				SplayTreeRotator<T>::rotate_left(node->parent, root);
			}
			else if(utils::is_left_son_of_right_son(node))
			{ // Zig-zag.
				SplayTreeRotator<T>::rotate_right(node->parent, root);
				SplayTreeRotator<T>::rotate_left(node->parent, root);
			
			}
			else if(utils::is_right_son_of_left_son(node))
			{ // Zig-zag 2: The Zigpocalypse.
				SplayTreeRotator<T>::rotate_left(node->parent, root);
				SplayTreeRotator<T>::rotate_right(node->parent, root);
			}
			else
				DEBUG("Splay operation reached undefined state of nodes.");
		}
	}
};

/**
 * Naive implementation of the splay operation using a sequence of simple
 * rotations.
 */
template<typename T>
struct NaiveSplayPolicy
{
	/**
	 * Propagates a given node to the top of the tree.
	 */
	static void splay(Node<T>* node, Node<T>** root)
	{
		if(!node || !root || !node->parent)
			return;

		while(node->parent)
		{
			if(utils::is_left_son(node))
				SplayTreeRotator<T>::rotate_right(node->parent, root);
			else
				SplayTreeRotator<T>::rotate_left(node->parent, root);
		}
	}
};

/**
 * Auxiliary class that takes care of the assignment.
 * (== parsing, control, ...)
 */
template<typename T, typename SplayPolicy>
class Task
{
	public:
		/**
		 * Constructor.
		 * Param: Name of the input file.
		 * Param: Name of the output file.
		 */
		Task(const std::string& file_name, const std::string& out_file_name = "test.out")
			: input_{file_name},
			  output_{out_file_name}
		{ /* DUMMY BODY */ }

		/**
		 * Destructor.
		 */
		~Task()
		{
			input_.close();
			output_.close();
		}

		/**
		 * Parses and executes a sequence of instruction contained in
		 * the input file.
		 */
		void process()
		{
			std::string token{};
			std::size_t count{};

			input_ >> token;
			if(token != "#")
			{
				DEBUG("Invalid token #1: " + token + ".");
				return;
			}

			while(input_  >> count)
			{
				DEBUG("Starting a new batch of " + std::to_string(count)
					  + " instructions.");

				tree_ = std::make_unique<SplayTree<T, SplayPolicy>>();
				T key{};
				for(std::size_t i = 0; i < count; ++i)
				{
					input_ >> token;
					if(token == "I")
					{
						input_ >> key;
						tree_->insert(key);
					}
					else
					{
						DEBUG("Only " + std::to_string(i + 1)
							  + "inserts, expected " + std::to_string(count) + ".");
						break;
					}
				}

				std::size_t find_length{};
				std::size_t find_count{};
				while(input_ >> token && token == "F")
				{
					input_ >> key;
					(void)tree_->find(key);

					++find_count;
					find_length += tree_->length_of_last_find();
				}

				if(find_count > 0)
				{
					auto average_length = find_length / find_count;
					output_ << count << " " << average_length << std::endl;
				}
			}
		}

	private:
		/**
		 * Tree used to accomplish the task.
		 */
		std::unique_ptr<SplayTree<T, SplayPolicy>> tree_;

		/**
		 * Input file stream.
		 */
		std::ifstream input_;

		/**
		 * Output file stream.
		 */
		std::ofstream output_;
};

#if RUN_TESTS == 1
void test();
#endif

/**
 * Entry point of the program, executes tests
 * if needed and performs the task with both policies
 * on either a file given as the command line parameter
 * or the file "data.txt".
 */
int main(int argc, char** argv)
{
#if RUN_TESTS == 1
	test();
#endif
	std::string input{};
	std::string output{};

	if(argc > 1)
		input = argv[1];
	else
		input = "data.txt";
	output = input.substr(0, input.size() - 4) + ".out";

	do
	{
		Task<int, DoubleRotationSplayPolicy<int>> double_task{input, "double-" + output};
		double_task.process();
	}
	while(false);

	do
	{
		Task<int, NaiveSplayPolicy<int>> naive_task{input, "naive-" + output};
		naive_task.process();
	}
	while(false);
};

#if RUN_TESTS == 1
bool test_1();
bool test_2();
bool test_3();
bool test_4();
bool test_5();

/**
 * A simple test suite that ensured I didn't
 * break any functionality of the splay tree.
 */
void test()
{
	if(test_1())
		TEST("Success.", 1);
	else
		TEST("Failure.", 1);

	if(test_2())
		TEST("Success.", 2);
	else
		TEST("Failure.", 2);

	if(test_3())
		TEST("Success.", 3);
	else
		TEST("Failure.", 3);

	if(test_4())
		TEST("Success.", 4);
	else
		TEST("Failure.", 4);

	if(test_5())
		TEST("Success.", 5);
	else
		TEST("Failure.", 5);
}

/**
 * Test of the left rotation on a small input.
 */
bool test_1()
{
	/**
	 * Nodes are statically allocated to
	 * use RAII.
	 */
	Node<int> node_1{1};
	Node<int> node_2{2};
	Node<int> node_3{3};
	Node<int> node_4{4};
	Node<int> node_5{5};

	/**
	 * Building the tree for rotation:
	 *     1          2
	 *    / \        / \
	 *   2  3   =>  1   5
	 *     / \     / \
	 *    4   5   3   4
	 */
	Node<int>* root{&node_1};
	root->left = &node_2;
	root->right = &node_3;
	root->right->left = &node_4;
	root->right->right = &node_5;

	SplayTreeRotator<int>::rotate_left(root, &root);

	bool res_root{root == &node_3};
	bool res_left{root->left == &node_1};
	bool res_left_left{root->left->left == &node_2};
	bool res_left_right{root->left->right == &node_4};
	bool res_right{root->right == &node_5};
	
	if(!res_root)
	{
		TEST("New root is incorrect: " + std::to_string(root->key)
			 + " != 3.", 1);
	}

	if(!res_left)
	{
		TEST("Left child of root is incorrect: "
			 + std::to_string(root->left->key) + " != 1.", 2);
	}

	if(!res_left_left)
	{
		TEST("Left-left grandchild of root is incorrect: "
			 + std::to_string(root->left->left->key) + " != 2.", 2);
	}

	if(!res_left_right)
	{
		TEST("Left-right grandchild of root is incorrect: "
			 + std::to_string(root->left->right->key) + " != 4.", 2);
	}

	if(!res_right)
	{
		TEST("Right child of root is incorrect: "
			 + std::to_string(root->right->key) + " != 5.", 2);
	}

	return res_root && res_left && res_left_left
		   && res_left_right && res_right;
}

/**
 * Test of the right rotation on a small input.
 */
bool test_2()
{
	/**
	 * Nodes are statically allocated to
	 * use RAII.
	 */
	Node<int> node_1{1};
	Node<int> node_2{2};
	Node<int> node_3{3};
	Node<int> node_4{4};
	Node<int> node_5{5};

	/**
	 * Building the tree for rotation:
	 *     1          2
	 *    / \        / \
	 *   2  3   =>  4   1
	 *  / \            / \
	 *  4  5          5   3
	 */
	Node<int>* root{&node_1};
	root->left = &node_2;
	root->right = &node_3;
	root->left->left = &node_4;
	root->left->right = &node_5;

	SplayTreeRotator<int>::rotate_right(root, &root);

	bool res_root{root == &node_2};
	bool res_left{root->left == &node_4};
	bool res_right_left{root->right->left == &node_5};
	bool res_right_right{root->right->right == &node_3};
	bool res_right{root->right == &node_1};
	
	if(!res_root)
	{
		TEST("New root is incorrect: " + std::to_string(root->key)
			 + " != 3.", 1);
	}

	if(!res_left)
	{
		TEST("Left child of root is incorrect: "
			 + std::to_string(root->left->key) + " != 4.", 1);
	}

	if(!res_right_left)
	{
		TEST("Left-left grandchild of root is incorrect: "
			 + std::to_string(root->right->left->key) + " != 5.", 1);
	}

	if(!res_right_right)
	{
		TEST("Left-right grandchild of root is incorrect: "
			 + std::to_string(root->right->right->key) + " != 3.", 1);
	}

	if(!res_right)
	{
		TEST("Right child of root is incorrect: "
			 + std::to_string(root->right->key) + " != 1.", 1);
	}

	return res_root && res_left && res_right_left
		   && res_right_right && res_right;
}

/**
 * Simple test that checks that both contains and
 * find functions work correctly and also checks if
 * insert builds a correct binary search tree.
 * NOTE: Double rotation version.
 */
bool test_3()
{
	SplayTree<int, DoubleRotationSplayPolicy<int>> tree{};
	int test_data[] { 4, 3, 2, 1, 6, 7, 8, 5};

	for(auto data : test_data)
	{
		tree.insert(data);
		tree.print();
	}
	if(!tree.validate())
		TEST("Tree invalid.", 3);

	bool res{true};
	for(auto data : test_data)
	{
		if(!tree.contains(data))
		{
			TEST("Tree does not contain key: " + std::to_string(data)
				 + ".", 3);
			res = false;
		}
	}

	for(auto data : test_data)
	{
		if(tree.find(data) != data)
		{
			TEST("Tree find failed: " + std::to_string(data)
				 + " != " + std::to_string(data) + ".", 3);
			res = false;
		}
	}

	return res;
}

/**
 * Simple test that checks that both contains and
 * find functions work correctly and also checks if
 * insert builds a correct binary search tree.
 * NOTE: Naive version.
 */
bool test_4()
{
	SplayTree<int, NaiveSplayPolicy<int>> tree{};
	int test_data[] { 4, 3, 2, 1, 6, 7, 8, 5};

	for(auto data : test_data)
		tree.insert(data);
	if(!tree.validate())
		TEST("Tree invalid.", 4);

	bool res{true};
	for(auto data : test_data)
	{
		if(!tree.contains(data))
		{
			TEST("Tree does not contain key: " + std::to_string(data)
				 + ".", 4);
			res = false;
		}
	}

	for(auto data : test_data)
	{
		if(tree.find(data) != data)
		{
			TEST("Tree find failed: " + std::to_string(data)
				 + " != " + std::to_string(data) + ".", 4);
			res = false;
		}
	}

	return res;
}

/**
 *
 */
bool test_5()
{
	std::string test_file{"test_x_a_b_11-_2444-_aafa.txt"};
	std::ofstream output{test_file};
	output << "# 3\nI 1\nI 2\nI 0\nF 1\nF 0\nF 2" << std::endl;

	Task<int, DoubleRotationSplayPolicy<int>> task1{test_file};
	task1.process();

	std::remove(test_file.c_str());

	return true;
}
#endif
