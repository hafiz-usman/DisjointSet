// DisjointSet.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <memory>
#include <unordered_map>

using namespace std;

// Disjoint sets using path compression and union by rank
// based on implementation from Tushar Roy: 
//      https://github.com/mission-peace/interview/blob/master/src/com/interview/graph/DisjointSet.java
//      https://www.youtube.com/watch?v=ID00PMy0-vE&feature=youtu.be

class DisjointSet
{
    struct Node
    {
        int val;
        int rank; // rank is used to help determine which node becomes root when performing union. And used in path compression
        Node* parent;
        Node() :
            val(-1),
            rank(-1),
            parent(nullptr)
        {}
    };

public:

    DisjointSet()
    {}

    ~DisjointSet()
    {}

    void makeSet(int data)
    {
        auto iter = _lut.find(data);
        if (iter != _lut.end())
        {
            // this already exists so nothing to do!
            return;
        }
        _lut[data] = make_unique<Node>();
        _lut[data]->val = data;
        _lut[data]->rank = INITIALRANK;
        _lut[data]->parent = _lut[data].get();
    }

    bool unionSet(int data1, int data2)
    {
        // Perform union by rank!
        auto iter1 = _lut.find(data1);
        auto iter2 = _lut.find(data2);
        if (iter1 == _lut.end() || 
            iter2 == _lut.end())
        {
            return false;
        }

        auto n1 = iter1->second.get();
        auto n2 = iter2->second.get();
        Node* parent1 = _findSetWithPathReduction(n1);
        Node* parent2 = _findSetWithPathReduction(n2);
        if (parent1 == parent2)
        {
            // nothing to do here since the two elements are already in the same set
            return true;
        }

        if (parent1->rank == parent2->rank)
        {
            // doesn't matter which tree you join to the other since they have the same rank

            // increment rank of combined tree since they had same rank
            parent1->rank++;
            parent2->parent = parent1;
        }
        else if (parent1->rank > parent2->rank)
        {
            // no need to update the rank
            parent2->parent = parent1;
        }
        else // parent1->rank < parent2->rank
        {
            parent1->parent = parent2;
        }

        return true;
    }

    bool findSet(int data, int& retData)
    {
        auto iter = _lut.find(data);
        if (iter == _lut.end())
        {
            return false;
        }
        Node* n1 = iter->second.get();
        Node* n2 = _findSetWithPathReduction(n1);
        retData = n2->val;
        return true;
    }

private:
    unordered_map<int, unique_ptr<Node>> _lut;
    const unsigned int INITIALRANK = 0;

    Node* _findSet(Node* node)
    {
        assert(node != nullptr);
        Node* parent = node->parent;
        while (node != node->parent)
        {
            node = node->parent;
        }
        return node;
    }

    Node* _findSetWithPathReduction(Node* node)
    {
        Node* parent = node->parent;
        if (node == parent)
        {
            return node;
        }
        node->parent = _findSetWithPathReduction(node->parent);
        return node->parent;
    }
};

const int MINVAL = 1;
const int MAXVAL = 7;

void baseTest(DisjointSet* ds)
{
    int val1 = -1;
    int val2 = -1;
    int retVal = -1;

    val1 = 0;
    val2 = 0;
    assert(ds->unionSet(val1, val2) == false);
    assert(ds->findSet(val1, retVal) == false);
    assert(ds->findSet(val2, retVal) == false);
    val1 = MINVAL;
    val2 = 0;
    assert(ds->unionSet(val1, val2) == false);
    assert(ds->findSet(val1, retVal) == true);
    assert(retVal == val1);
    assert(ds->findSet(val2, retVal) == false);
    val1 = 0;
    val2 = MINVAL;
    assert(ds->unionSet(val1, val2) == false);
    assert(ds->findSet(val1, retVal) == false);
    assert(ds->findSet(val2, retVal) == true);
    assert(retVal == val2);
    val1 = MINVAL;
    val2 = val1 + 1;
    assert(ds->unionSet(val1, val2) == true);
    assert(ds->findSet(val1, retVal) == true);
    assert(retVal == val1);
    assert(ds->findSet(val2, retVal) == true);
    assert(retVal == val1); // should be same as val1 due to unionSet()
}

int main()
{
    unique_ptr<DisjointSet> ds = make_unique<DisjointSet>();
    int val1 = -1;
    int val2 = -1;
    bool retFlag = false;
    int retVal = -1;

    for (int v = MINVAL; v <= MAXVAL; ++v)
    {
        retFlag = ds->findSet(v, retVal);
        assert(retFlag == false);

        ds->makeSet(v);

        retFlag = ds->findSet(v, retVal);
        assert(retFlag == true);
        assert(retVal == v);
    }

    baseTest(ds.get());

    val1 = 2;
    val2 = 3;
    assert(ds->unionSet(val1, val2) == true);
    assert(ds->findSet(val1, retVal) == true);
    assert(retVal == 1);
    assert(ds->findSet(val2, retVal) == true);
    assert(retVal == 1); // should be same as 1 due to unionSet()

    baseTest(ds.get());

    val1 = 4;
    val2 = 5;
    assert(ds->unionSet(val1, val2) == true);
    assert(ds->findSet(val1, retVal) == true);
    assert(retVal == 4);
    assert(ds->findSet(val2, retVal) == true);
    assert(retVal == 4); // should be same as 4 due to unionSet()

    baseTest(ds.get());

    val1 = 6;
    val2 = 7;
    assert(ds->unionSet(val1, val2) == true);
    assert(ds->findSet(val1, retVal) == true);
    assert(retVal == 6);
    assert(ds->findSet(val2, retVal) == true);
    assert(retVal == 6); // should be same as 6 due to unionSet()

    baseTest(ds.get());

    val1 = 5;
    val2 = 6;
    assert(ds->unionSet(val1, val2) == true);
    assert(ds->findSet(val1, retVal) == true);
    assert(retVal == 4);
    assert(ds->findSet(val2, retVal) == true);
    assert(retVal == 4); // should be same as 4 due to unionSet()

    baseTest(ds.get());

    val1 = 3;
    val2 = 7;
    assert(ds->unionSet(val1, val2) == true);
    assert(ds->findSet(val1, retVal) == true);
    assert(retVal == 4);
    assert(ds->findSet(val2, retVal) == true);
    assert(retVal == 4); // should be same as 4 due to unionSet()

    //baseTest(ds.get()); basetest isn't valid anymore cuz now 1's parent is 4

    return 0;
}

