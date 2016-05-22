#ifndef ELEMENTFREQUENCYTREE_H
#define ELEMENTFREQUENCYTREE_H


/**
 * @brief The ElementFrequencyTree class is a binary tree for gathering and storing statistics
 * of the different morse code characters in a format that's easy to access while decoding on the fly.
 */
class ElementFrequencyTree
{
public:
    ElementFrequencyTree();
    ElementFrequencyTree(ElementFrequencyTree* parent);
    ~ElementFrequencyTree();
    void addDit();
    void addDah();
    void addTermination();
    double ditRatio() const;
    double branchProbability() const;
    /**
     * @brief ditBranch returns the dit-branch of the tree, creates one if necessary
     * @return
     */
    ElementFrequencyTree& ditBranch();

    /**
     * @brief dahBranch returns the dah-branch of the tree, creates one if necessary
     * @return
     */
    ElementFrequencyTree& dahBranch();

    ElementFrequencyTree *root() const;

private:
    ElementFrequencyTree(const ElementFrequencyTree& other)=delete;
    /**
     * @brief totalDits_ counts the recorded amount of dits
     */
    int totalDits_;
    /**
     * @brief totalDahs_ counts the total recorded amount of dahs
     */
    int totalDahs_;
    /**
     * @brief characterTerminations_ counts the frequency of stops.
     */
    int characterTerminations_;
    ElementFrequencyTree* ditBranch_;
    ElementFrequencyTree* dahBranch_;
    /**
     * @brief base_ pointer to the root
     */
    ElementFrequencyTree* parent_;
};

#endif // ELEMENTFREQUENCYTREE_H
