/**
  ******************************************************************************
  * @file           : SkipList.h
  * @author         : zgys
  * @brief          : None
  * @attention      : None
  * @date           : 23-4-1
  ******************************************************************************
  */


#ifndef KVDB_SKIPLIST_H
#define KVDB_SKIPLIST_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace kvDB {

#define MAX_LEVEL 12

    class SkipListLevel;

    // class for Node
    class SkipListNode {
    public:
        SkipListNode() = default;

        SkipListNode(const std::string& obj, double score, int level);

        std::unique_ptr<std::unique_ptr<SkipListLevel>[]> levels_;
        std::string obj_;
        double score_;
    };

    // class for level
    class SkipListLevel {
    public:
        SkipListLevel() : forward_(nullptr) {}

        SkipListNode* forward_;
    };

    // class for range
    class RangeSpec {
    public:
        RangeSpec(double min, double max)
                : min_(min), max_(max),
                  minex_(true), maxex_(true) {}

        double min_, max_;
        bool minex_, maxex_;
    };

    class SkipList {
    public:
        SkipList();

        ~SkipList();

        // noncopyable
        SkipList(SkipList &) = delete;

        SkipList& operator=(SkipList &) = delete;

        /* 创建跳表节点 */
        SkipListNode* createNode(const std::string& obj, double score, int level);
        /* 插入数据时level是否上升，以 1/4 的概率选择 */
        int getRandomLevel();
        /* 插入节点 */
        void insertNode(const std::string&, double);
        /* 删除节点 */
        void deleteNode(const std::string&, double);
        unsigned long getCountInRange(RangeSpec & range);
        std::vector<SkipListNode*> getNodeInRange(RangeSpec & range);

        unsigned long getLength() { return length_; }

    private:
        SkipListNode* header_;  // 跳表头节点
        // 用来保证key不同
        std::unordered_map<std::string, double> keySet_;

        int           level_;   // 跳表的层高
        unsigned long length_;

        int valueGteMin(double value, RangeSpec& spec);
        int valueLteMax(double value, RangeSpec& spec);
    };
}

#endif //KVDB_SKIPLIST_H
