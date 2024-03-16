## [23. 合并 K 个升序链表](https://leetcode.cn/problems/merge-k-sorted-lists/)

[https://leetcode.cn/problems/merge-k-sorted-lists/](https://leetcode.cn/problems/merge-k-sorted-lists/) 该题需要动态插入式进行排序，比较高效的办法是使用优先级队列，而优先级队列有不同的实现方式，其中比较高效的就是二叉堆方式。

```cpp
class Solution {  
​  
public:  
    ListNode* mergeKLists(vector<ListNode*>& lists) {  
        ListNode *dummy = new ListNode;  
        ListNode *p = dummy;  
        priority_queue<ListNode*, vector<ListNode*>, function<bool(ListNode*, ListNode*)>> pq(  
        [] (ListNode* a, ListNode* b) { return a->val > b->val; });  
​  
        for(auto head : lists){  
            if(head!=NULL)  
                pq.push(head);  
        }  
        while(!pq.empty()){  
            ListNode *min = pq.top();  
            p->next = min;  
            p = p->next;  
            pq.pop();  
            if(min->next!=NULL)  
                pq.push(min->next);  
        }  
        return dummy->next;  
    }  
};
```

## [19. 删除链表的倒数第 N 个结点](https://leetcode.cn/problems/remove-nth-node-from-end-of-list/)

通过dummy来解决删除链表头的情况，减少考虑边界条件。**只要产生新链表，就以dummy作为新链表的头**

```cpp
class Solution {  
public:  
    ListNode* removeNthFromEnd(ListNode* head, int n) {  
        ListNode* dummy = new ListNode(-1);  
        ListNode* slow = dummy;  
        ListNode* fast = head;  
        dummy->next = head;  
        for (int i = 0; i < n; i++) {  
            fast = fast->next;  
        }  
​  
        while (fast) {  
            fast = fast->next;  
            slow = slow->next;  
        }  
        slow->next = slow->next->next;  
        return dummy->next;  
    }  
};
```

## [142. 环形链表 II](https://leetcode.cn/problems/linked-list-cycle-ii/)

![[Pasted image 20240224133400.png]]
判断环形链表的方法是快慢指针 ![[leetcode/img/Pasted image 20240224140153.png]]

```cpp
class Solution {  
public:  
    ListNode* detectCycle(ListNode* head) {  
        ListNode* fast = head, * slow = head;  
​  
        while (fast != NULL && fast->next != NULL) {  
            fast = fast->next->next;  
            slow = slow->next;  
            if (fast == slow)  
                break;  
        }  
        if (fast == nullptr || fast->next == nullptr) {  
            // fast 遇到空指针说明没有环  
            return nullptr;  
        }  
​  
        slow = head;  
​  
        while (slow != fast) { //一定要靠前判断，不能在赋值后判断，不然可能错过链表头为结果的情况  
            slow = slow->next;  
            fast = fast->next;  
        }  
        return slow;  
    }  
};
```
## [25. K 个一组翻转链表](https://leetcode.cn/problems/reverse-nodes-in-k-group/)

```cpp
class Solution {  
public:  
    ListNode* reverseKGroup(ListNode* head, int k) {  
        int i=k;  
        ListNode* pr=head;  
        while(i--){  
            if(pr==nullptr)  
                return head;  
            pr = pr->next;  
        }  
        ListNode* pnexthead = reverseA2B(head, pr);  
        head->next = reverseKGroup(pr,k);  
        return pnexthead;  
    }  
    ListNode* reverseA2B(ListNode* A, ListNode* B){  
        ListNode *pnow = A;  
        ListNode *pnext=nullptr;  
        ListNode *ppriv=nullptr;  
        while(pnow!=B){  
            pnext = pnow->next;  
            pnow->next = ppriv;  
            ppriv = pnow;  
​  
            pnow = pnext;  
        }  
        return ppriv;  
    }  
};
```

## [234. 回文链表](https://leetcode.cn/problems/palindrome-linked-list/)

该题目的思路是要做到后序遍历，如果想正序访问链表，那可以前序遍历，如果想倒序访问链表，那就得用后序遍历。

```cpp
/* 倒序打印单链表中的元素值 */  
void traverse(ListNode* head) {  
    if (head == NULL) return;  
    traverse(head->next);  
    // 后序遍历代码  
    print(head->val);  
}

class Solution {  
public:  
    ListNode* left;  
​  
    bool isPalindrome(ListNode* head) {  
        left = head;  
        return reverse(head);  
    }  
    bool reverse(ListNode* right){  
        if(right==nullptr)  
            return true;  
          
        bool res = reverse(right->next);  
        res = res&&(right->val==left->val);  
          
          
        left = left->next;  
        return res;  
    }  
};
```
## [26. 删除有序数组中的重复项](https://leetcode.cn/problems/remove-duplicates-from-sorted-array/)
```c
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        int slow=0,fast=0;
        
        while(fast<nums.size()){
            if(nums[fast]==nums[slow]){
                fast++;
            }else{
                slow++;
                nums[slow] = nums[fast];
                fast++;
            }
        }
        return slow+1;
    }
};
```
## [83. 删除排序链表中的重复元素](https://leetcode.cn/problems/remove-duplicates-from-sorted-list/)
```c
class Solution {
public:
    ListNode* deleteDuplicates(ListNode* head) {
        ListNode* fast = head;
        ListNode* slow = head;
        if(head==nullptr)
            return nullptr;
        while(fast!=nullptr){
            if(fast->val == slow->val){
                fast = fast->next;
            }else{
                slow->next = fast;
                slow = slow->next;
                fast = fast->next;
            }
        }
        slow->next = nullptr;
        return head;
    }
};
```
记得要把慢指针最后断开，不然还会连着后面的节点。
## [27. 移除元素](https://leetcode.cn/problems/remove-element/)
```c
class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        int fast=0,slow=0;
        while(fast!=nums.size()){
            if(nums[fast]==val){
                fast++;
            }
            else{
                nums[slow++] = nums[fast];
                fast++;
            }
        }
        return slow;
    }
};
```
## [283. 移动零](https://leetcode.cn/problems/move-zeroes/)
```c
class Solution {
public:
    void moveZeroes(vector<int>& nums) {
        int slow=0,fast=0;
        if(nums.size()==1)
            return;
        while(fast!=nums.size()){
            if(nums[fast]==0){
                fast++;
            }else{
                nums[slow++]=nums[fast];
                fast++;
            }
        }
        memset(&nums[slow], 0, (nums.size()-slow)*sizeof(int));
    }
};
```
## [167. 两数之和 II - 输入有序数组](https://leetcode.cn/problems/two-sum-ii-input-array-is-sorted/)
```c
class Solution {
public:
    vector<int> twoSum(vector<int>& numbers, int target) {
        int left=0,right=numbers.size()-1;
        while(1){
            int sum = numbers[left]+numbers[right];
            if(sum==target){
                return {left+1,right+1};
            }else if(sum<target){
                left++;
            }else{
                right--;
            }
        }

    }
};
```
## [344. 反转字符串](https://leetcode.cn/problems/reverse-string/)

```c
class Solution {
public:
    void reverseString(vector<char>& s) {
        int left=0,right=s.size()-1;
        
        while(right>left){
           char temp=s[left];
           s[left] = s[right];
           s[right] = temp; 
           right--;
           left++;
        }
    }
};
```
## [303. 区域和检索 - 数组不可变](https://leetcode.cn/problems/range-sum-query-immutable/)
该题主要是查询费时间，如果用循环，则每次查询时间复杂度O(N)，不行，所以可以在初始化的时候构建前缀表达式，这样后面查询的时候就快了。
```c
class NumArray {
public:
    NumArray(vector<int>& nums) {
        pre_sum.resize(nums.size()+1);
        for(int i=1;i<pre_sum.size();i++){
            pre_sum[i] = pre_sum[i - 1] + nums[i - 1];
        }
    }
    
    int sumRange(int left, int right) {
        return pre_sum[right+1]-pre_sum[left];
    }
private:
    vector<int> pre_sum;
};
```
## [304. 二维区域和检索 - 矩阵不可变](https://leetcode.cn/problems/range-sum-query-2d-immutable/)
```c
class NumMatrix {
private:
    // 定义：preSum[i][j] 记录 matrix 中子矩阵 [0, 0, i-1, j-1] 的元素和
    vector<vector<int>> preSum;
    
public:
    NumMatrix(vector<vector<int>>& matrix) {
        int m = matrix.size(), n = matrix[0].size();
        if (m == 0 || n == 0) return;
        // 构造前缀和矩阵
        preSum = vector<vector<int>>(m + 1, vector<int>(n + 1));
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                // 计算每个矩阵 [0, 0, i, j] 的元素和
                preSum[i][j] = preSum[i-1][j] + preSum[i][j-1] + matrix[i - 1][j - 1] - preSum[i-1][j-1];
            }
        }
    }
    
    // 计算子矩阵 [x1, y1, x2, y2] 的元素和
    int sumRegion(int x1, int y1, int x2, int y2) {
        // 目标矩阵之和由四个相邻矩阵运算获得
        return preSum[x2+1][y2+1] - preSum[x1][y2+1] - preSum[x2+1][y1] + preSum[x1][y1];
    }
};
```
这类前缀和的题，创建前缀和数组的时候可以把前缀和数组长度开大1，这样后面计算相减的时候可以比较简单，不用考虑边界条件。
## [1109. 航班预订统计](https://leetcode.cn/problems/corporate-flight-bookings/)
```c
class Solution {
private:
    vector<int> diff;
public:
    void diff_add(vector<int> &booking){
        diff[booking[0]-1]  += booking[2];
        if(booking[1]<diff.size())
            diff[booking[1]]    -= booking[2];
    }
    vector<int> diff2normal(vector<int> &diff){
        vector<int> normal = diff;
        for(int i=1; i<diff.size(); i++){
            normal[i] = diff[i]+normal[i-1];
        }
        return normal;
    }
    vector<int> corpFlightBookings(vector<vector<int>>& bookings, int n) {
        diff = vector<int>(n,0);
        for(int i=0; i<bookings.size(); i++){
            diff_add(bookings[i]);
        }
        return diff2normal(diff);
    }
};
```
使用差分数组，画画图就懂了
## [1094. 拼车](https://leetcode.cn/problems/car-pooling/)
```c
class Solution {
private:
    vector<int> diff;
public:
    void diff_add(vector<int> &trip){
        diff[trip[1]]  += trip[0];
        if(trip[2]<diff.size())
            diff[trip[2]]    -= trip[0];
    }
    vector<int> diff2normal(vector<int> &diff){
        vector<int> normal = diff;
        for(int i=1; i<diff.size(); i++){
            normal[i] = diff[i]+normal[i-1];
        }
        return normal;
    }
    bool carPooling(vector<vector<int>>& trips, int capacity) {
        int max = -1;
        for(int i=0; i<trips.size(); i++){
            if(trips[i][2]>max)
                max = trips[i][2];
        }

        diff.resize(max);
        
        for(int i=0; i<trips.size(); i++)
            diff_add(trips[i]);

        vector<int> normal = diff2normal(diff);
        for(int i=0; i<normal.size(); i++){
            if(normal[i]>capacity)
                return false;
        }
        return true;

    }
};
```
## [48. 旋转图像](https://leetcode.cn/problems/rotate-image/)
二维矩阵顺时针旋转，可以看作先转置，再把列互换。
```c
class Solution {
public:
    void Trans(vector<vector<int>> &matrix){
        for(int i=0; i<matrix.size(); i++)
            for(int j=i; j<matrix[0].size(); j++){
                swap(matrix[i][j],matrix[j][i]);
            }
    }
    void Mirror(vector<vector<int>>& matrix){
        for(int i=0; i<matrix[0].size()/2; i++)
            for(int j=0; j<matrix.size(); j++){
                swap(matrix[j][i], matrix[j][matrix[0].size()-i-1]);
            }
    }
    void rotate(vector<vector<int>>& matrix) {
        Trans(matrix);
        Mirror(matrix);
    }
};
```
## [59. 螺旋矩阵 II](https://leetcode.cn/problems/spiral-matrix-ii/)
通过两个标志位行和列，每次遍历一行后，把列-1，遍历一列后，把行-1.这样就可以实现螺旋遍历。
```cpp
class Solution {
public:
    vector<vector<int>> generateMatrix(int n) {
    #define mrignt  0
    #define mdown   1
    #define mleft   2
    #define mup     3

        int state = mrignt;
        int h = n;
        int z = n;

        int x = 0, y = 0;
        int start = 1;
        vector<vector<int>> matrix(n);
        for(int i=0;i<n;i++)
            matrix[i].resize(n);

        matrix[0][0]=1;
        int phase = 1;

        while (1) {
            switch (state) {
            case mrignt:
                if (start == 1) {
                    start = 0;
                    for (int i = 0; i < h - 1; i++) {
                        matrix[x][++y]=++phase;
                    }
                }
                else {
                    for (int i = 0; i < h; i++) {
                        matrix[x][++y]=++phase;
                    }
                }
                z--;
                state = mdown;
                break;
            case mdown:
                for (int i = 0; i < z; i++) {
                    matrix[++x][y] = ++phase;
                }
                state = mleft;
                h--;
                break;
            case mleft:
                for (int i = 0; i < h; i++) {
                    matrix[x][--y] = ++phase;
                }
                state = mup;
                z--;
                break;
            case mup:
                for (int i = 0; i < z; i++) {
                    matrix[--x][y] = ++phase;
                }
                state = mrignt;
                h--;
                break;
            }
            if (h == 0 || z == 0)
                return matrix;
        }

    }
};
```
## [76. 最小覆盖子串](https://leetcode.cn/problems/minimum-window-substring/)
使用滑动窗口，要考虑窗口什么时候放大，什么时候缩小。
```cpp
class Solution {
public:
    string minWindow(string s, string t) {
        unordered_map <char,int> need, windows;
        int left=0, right=0, valid=0, start=0, len=INT_MAX;
        
        //初始化需求表
        for (auto c : t)
            need[c]++;

        //维护滑动窗口
        while (right < s.size()) {
            char c = s[right];
            right++;
            if (need.count(c)) {      //如果属于需求，压入滑动窗口
                windows[c]++;
                if (windows[c] == need[c])
                    valid++;    //如果窗口中某字符出现次数等于need中该字符出现次数，valid++
            }

            while (valid == need.size()) {  //找到子串
                if (right - left < len) {   //更新最小子串
                    start = left;
                    len = right - left;
                }
                char c = s[left];           
                if (need.count(c)) {
                    windows[c]--;
                    if (windows[c] < need[c]) {
                        valid--;
                    }
                }

                left++;
                
            }


        }
        if (len == INT_MAX)
            return "";
        else
            return s.substr(start,len);
    }
};
```
需要注意的是，unordered_map中，直接使用[]索引，会添加该索引项，单纯想判断是否存在某项的话，应该用count()方法。
## [567. 字符串的排列](https://leetcode.cn/problems/permutation-in-string/)
同样使用滑动窗口，只是一直把窗口大小保持为s1的长度
```cpp
class Solution {
public:
    bool checkInclusion(string s1, string s2) {
        int left = 0, right = 0, valid = 0;
        unordered_map<char, int> need, windows;

        for (auto c : s1) need[c]++;
        
        while (right < s2.size()) {
            char c = s2[right];
            if (need.count(c)) {
                windows[c]++;
                if (windows[c] == need[c])
                    valid++;
            }
            right++;

            while (right - left >= s1.size()) {
                char c = s2[left];
                if (valid == s1.size())
                    return true;
                
                if (need.count(c)) {
                    windows[c]--;
                    valid--;
                }  
                left++;
            }
        
        }
        return false;
    }
};
```

## [438. 找到字符串中所有字母异位词](https://leetcode.cn/problems/find-all-anagrams-in-a-string/)
```cpp
class Solution {
public:
    vector<int> findAnagrams(string s, string p) {
        vector<int> res;
        int left=0, right=0, valid=0;
        unordered_map<char, int> need, windows;
        for(auto c:p) need[c]++;
        
        while(right<s.size()){
            char c = s[right];
            if(need.count(c)){
                windows[c]++;
                if(need[c]==windows[c])
                    valid++;
            }

            right++;

            while(right-left == p.size()){
                char c = s[left];

                if(valid==need.size())
                    res.push_back(left);

                left++;

                if(need.count(c)){
                    if(windows[c]==need[c])
                        valid--;
                    windows[c]--;
                }


            }
        }
        return res;
    }
};
```
## [3. 无重复字符的最长子串](https://leetcode.cn/problems/longest-substring-without-repeating-characters/)
与前面稍有不同，这个题主要是要考虑清楚，一旦right重复了，left要一直递增到不重复为止，前面的就都废了。
```cpp
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int left=0, right=0,res=0;
        unordered_map<char, int> windows;

        while(right<s.size()){
            char c = s[right];
            right++;
            windows[c]++;

            
            while(windows[c]>1){
                char d = s[left];
                left++;
                windows[d]--;
            }
            res = max(res,right-left);

        }
        return res;
    }
};
```
## [704. 二分查找](https://leetcode.cn/problems/binary-search/)
```cpp
class Solution {
public:
    int search(vector<int>& nums, int target) {
        int left=0,mid=0;
        int right = nums.size() - 1;
        while(right>=left){
            mid = left+(right-left)/2;
            if(nums[mid]==target)
                return mid;
            else if(nums[mid]>target)
                right=mid-1;
            else if(nums[mid]<target)
                left=mid+1;
        }
        return -1;
    }
};
```
`int right = nums.size() - 1;`这句含义是搜索区间为左闭右闭区间，可以直接索引区间边界。
`mid = left+(right-left)/2;`这句含义是找区间中点，不用`(right+left)/2`是因为容易溢出。
`right=mid-1;`减一是因为前面已经确定过mid不是target，而且是闭区间，所以可以索引mid

## [380. O(1) 时间插入、删除和获取随机元素](https://leetcode.cn/problems/insert-delete-getrandom-o1/)
O1插入，删除很容易想到哈希表，但是如果想获取所及元素O1就不能了，因为哈希表不是顺序容器，没法直接访问，所以需要再开个数组，用来获取随机元素。
```cpp
class RandomizedSet {
public:
    vector<int> nums;
    unordered_map <int, int> val2idx; 
    RandomizedSet() {
    }
    
    bool insert(int val) {
        if(val2idx.count(val)){
            return false;
        }
        val2idx[val] = nums.size();
        nums.push_back(val);
        return true;
    }
    
    bool remove(int val) {
        if(!val2idx.count(val)){
            return false;
        }
        int idx_val = val2idx[val];
        val2idx[nums.back()] = idx_val;
        nums[idx_val] = nums.back();
        nums.pop_back();
        val2idx.erase(val);
        return true;
    }
    
    int getRandom() {
        return nums[random()%nums.size()];
    }
};
```
## [710. 黑名单中的随机数](https://leetcode.cn/problems/random-pick-with-blacklist/)
同样是利用哈希表来实现O1读。这里不用数组，只是实现一个映射在数字末尾虚拟出一个白名单，是因为如果用数组存储一个白名单的话，n大，如果这时候黑名单很小，那白名单就会很大（想想n=1000,blacklist={0}，那白名单size=999）利用率很低，所以不能把白名单存储下来。
```cpp
class Solution {
public:
    unordered_map<int, int> mapping;
    int w;
    Solution(int n, vector<int>& blacklist) {
        for(auto b:blacklist) mapping[b]=0;
        int last = n-1;
        w=n-blacklist.size();
        for(auto b:blacklist){
            if(b<n-blacklist.size()){
                //如果在前部分，就映射
                while(mapping.count(last)){
                    //如果目前的last在黑名单，就把last左移
                    last--;
                }
                mapping[b]=last;
                last--;
            }else{
                //在后半部分，不用映射
            }
        }
    }
    
    int pick() {
        int r = random()%w;
        if(mapping.count(r)){
            return mapping[r];
        }
        return r;
    }
};
```
## [104. 二叉树的最大深度](https://leetcode.cn/problems/maximum-depth-of-binary-tree/)
最大深度有几种解法，可以前序遍历，找一个外部变量存储最大深度和当前深度，也可以后序遍历，从枝叶部开始返回，每次返回+1并判断左支和右支哪个更深，从而得到最大深度。
```cpp
class Solution {
public:
    int depth=0;
    int max;
    void tranverse(TreeNode* root){
        if(!root)
            return;
        
        depth++;
        if((root->left==nullptr)&&(root->right==nullptr))
            max = depth>max?depth:max;
        
        tranverse(root->left);
        tranverse(root->right);
        depth--;
    }
    int maxDepth(TreeNode* root) {
        tranverse(root);
        return max;
    }
};
```
## [144. 二叉树的前序遍历](https://leetcode.cn/problems/binary-tree-preorder-traversal/)
```cpp
class Solution {
public:
    vector <int> res;
    void traverse(TreeNode* root){
        if(!root)
            return;
        res.push_back(root->val);
        traverse(root->left);
        traverse(root->right);
        return;
    }
    vector<int> preorderTraversal(TreeNode* root) {
        traverse(root);
        return res;
    }
};
```
## [543. 二叉树的直径](https://leetcode.cn/problems/diameter-of-binary-tree/)
这道题首先明确直径的含义，直径就是整个叶子节点之间最短距离的最大值，所以第一个思路就是遍历整个二叉树，在节点上再进行遍历，获取左侧枝子的最深深度，和右侧枝子的最深深度，然后更新最大直径。这样慢，慢是因为前序遍历无法直接获取到叶子节点的信息（因为还没遍历到），所以只好采用再调用一个递归函数来获取整个枝子的信息，所以慢：
```cpp
class Solution {
public:
    int max_width=0;
    void traverse(TreeNode* root){
        if(!root)
            return;
        
        int left = max_depth(root->left);
        int right = max_depth(root->right);

        max_width = max(max_width, left+right);

        traverse(root->left);
        traverse(root->right);        
    }
    int max_depth(TreeNode* root){
       if(!root)
        return 0;

        int left_max = max_depth(root->left)+1;
        int right_max = max_depth(root->right)+1;
        return max(left_max,right_max); 
    }
    int diameterOfBinaryTree(TreeNode* root) {
        traverse(root);
        return max_width;
    }
};
```
改进办法是利用后序遍历，后序遍历的时候，已经完成了整个枝遍历，所以可以获取到叶子的信息。
```cpp
class Solution {
public:
    int max_width=0;
    int traverse(TreeNode* root){
        if(!root)
            return 0;
        int left = traverse(root->left);
        int right = traverse(root->right);

        max_width = max(max_width, left+right);

        return max(left,right)+1;
    }
    int diameterOfBinaryTree(TreeNode* root) {
        traverse(root);
        return max_width;
    }
};
```
## [226. 翻转二叉树](https://leetcode.cn/problems/invert-binary-tree/)
```cpp
class Solution {
public:
    void traverse(TreeNode* root){
        if(!root)   return;
        
        swap(root->left,root->right);
        traverse(root->left);
        traverse(root->right);
    }
    TreeNode* invertTree(TreeNode* root) {
        traverse(root);
        return root;
    }
};
```
反转二叉树，可以用前序遍历，遍历每一个节点，把其子节点交换。也可以采用后序遍历，swap后直接返回当前root。
```cpp
class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
        if(!root) return nullptr;

        TreeNode* left = invertTree(root->left);
        TreeNode* right = invertTree(root->right);

        swap(root->left, root->right);
        return root;
    }
};
```
## [116. 填充每个节点的下一个右侧节点指针](https://leetcode.cn/problems/populating-next-right-pointers-in-each-node/)
```cpp
class Solution {
public:
    Node* connect(Node* root) {
        if(root==nullptr) return root;
        traverse(root->left,root->right);
        return root;
    }
    void traverse(Node* left, Node* right){
        if((left==nullptr)||(right==nullptr)) return;

        left->next = right;

        traverse(left->left,left->right);
        traverse(right->left,right->right);
        traverse(left->right,right->left);
    }
};
```
![[Pasted image 20240314094230.png]]
不仅要对45，67之间链接，还要对56之间连接，所以才写成以上形式。
## [114. 二叉树展开为链表](https://leetcode.cn/problems/flatten-binary-tree-to-linked-list/)
这个题用前序遍历不好办，因为不能新建个链表，必须原位更新，可以使用后序遍历，正向思维，先把左子树和右子树分别拉平，再把右子树接到左子树末端，最后把左子树清空。
```cpp
class Solution {
public:
    void flatten(TreeNode* root) {
        if(!root)   return;
        //先把左右子树分别拉平
        flatten(root->left);
        flatten(root->right);
        
        //先找到左子树末端
        TreeNode* left_end;
        left_end = root->left;
        if(left_end){ //如果左子树不为空，则执行，如果左子树本身为空，就不用管了
            while(left_end->right){
                left_end = left_end->right;
            }
            //把右子树接到左子树末端
            left_end->right = root->right;
            //把左子树移动到右子树，并把左子树清空
            root->right =root->left;
            root->left=nullptr;
        }
    }
};
```
## [654. 最大二叉树](https://leetcode.cn/problems/maximum-binary-tree/)
牛逼！独立写出了这个递归，其实思想简单，就考虑当下就可以，最近题没白刷
```cpp
class Solution {
public:
    TreeNode* traverse(vector<int>& nums, int start, int end){
        int max=0,max_idx=0;
        TreeNode *res = new TreeNode();
        if(start>=end) return nullptr;
        
        for(int i=start;i<end;i++)
            if(nums[i]>=max){
                max = nums[i];
                max_idx = i;
            }
        
        TreeNode* left_tree     = traverse(nums, start, max_idx); //构造左子树
        TreeNode* right_tree    = traverse(nums, max_idx+1, end); //构造右子树

        res->val = max;
        res->left = left_tree;
        res->right = right_tree;
        return res;
    }
    TreeNode* constructMaximumBinaryTree(vector<int>& nums) {
        return traverse(nums, 0, nums.size());
    }
};
```
## [105. 从前序与中序遍历序列构造二叉树](https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-inorder-traversal/)
这道题的思路就是利用两种遍历模式，首先在前序遍历中找根节点，然后在后序遍历中分成左右子树，注意递归函数传入的参数：
```cpp
class Solution {
public:
    TreeNode* builtTree(vector<int>& preorder, int pre_start, int pre_end, vector<int>& inorder, int in_start, int in_end){
        if((pre_start>=pre_end)||(in_start>=in_end)) return nullptr;
        int root_val = preorder[pre_start];
        int in_root_idx;
        for(int i=in_start; i<in_end; i++){
            if(root_val == inorder[i]){
                in_root_idx = i;
            }
        }





        TreeNode* root_left  = builtTree(preorder,pre_start+1,(pre_start+1+(in_root_idx-in_start)),inorder,in_start,in_root_idx);
        TreeNode* root_right = builtTree(preorder,(pre_start+1+(in_root_idx-in_start)),pre_end,inorder,in_root_idx+1,in_end);
        
        TreeNode* root = new TreeNode(root_val, root_left, root_right);
        return root;
    }
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        return builtTree(preorder,0,preorder.size(),inorder,0,inorder.size());
    }
};
```
## [106. 从中序与后序遍历序列构造二叉树](https://leetcode.cn/problems/construct-binary-tree-from-inorder-and-postorder-traversal/)
```cpp
class Solution {
public:
    unordered_map<int, int> in_map;
    TreeNode* builtTree(vector<int>& inorder, int in_start, int in_end, vector<int>& postorder, int post_start, int post_end){
        if((post_start>=post_end)||(in_start>=in_end)) return nullptr;
        int root_val = postorder[post_end-1];
        int in_root_idx = in_map[root_val];

        
        TreeNode* root_left  = builtTree(inorder,in_start,in_root_idx,postorder,post_start,post_start+(in_root_idx-in_start));
        TreeNode* root_right = builtTree(inorder,,?,postorder,?,?);
        
        TreeNode* root = new TreeNode(root_val, root_left, root_right);
        return root;
    }
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        for(int i=0;i<inorder.size(); i++)
            in_map[inorder[i]] = i;
            
        return builtTree(inorder,0,inorder.size(), postorder, 0, postorder.size());
    }
};
```
## [106. 从中序与后序遍历序列构造二叉树](https://leetcode.cn/problems/construct-binary-tree-from-inorder-and-postorder-traversal/)
```cpp
class Solution {
public:
    unordered_map<int, int> in_map;
    TreeNode* builtTree(vector<int>& inorder, int in_start, int in_end, vector<int>& postorder, int post_start, int post_end){
        if((post_start>post_end)||(in_start>in_end)) return nullptr;
        int root_val = postorder[post_end];
        int in_root_idx = in_map[root_val];

        
        TreeNode* root_left  = builtTree(inorder,in_start,in_root_idx-1,postorder,post_start,post_start-1+(in_root_idx-in_start));
        TreeNode* root_right = builtTree(inorder,in_root_idx+1,in_end,postorder,post_start+(in_root_idx-in_start),post_end-1);
        
        TreeNode* root = new TreeNode(root_val, root_left, root_right);
        return root;
    }
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        for(int i=0;i<inorder.size(); i++)
            in_map[inorder[i]] = i;
            
        return builtTree(inorder,0,inorder.size()-1, postorder, 0, postorder.size()-1);
    }
};
```
## [222. 完全二叉树的节点个数](https://leetcode.cn/problems/count-complete-tree-nodes/)
是一颗完全二叉树，可能有奇技淫巧，但是先实现最简单的吧：
```cpp
class Solution {
public:
    int num=0;
    int countNodes(TreeNode* root) {
        if(!root) return 0;

        int a = countNodes(root->left);
        int b = countNodes(root->right);
        return a+b+1;
    }
};
```
改进一下，在递归每个节点时，判断其左右子树的叶子深度是不是一致的，如果一致，说明这个子节点对应的子树是满二叉树，可以直接用指数来求。
```cpp
class Solution {
public:
    int num=0;
    int countNodes(TreeNode* root) {
        if(!root) return 0;
        int dl=0, dr=0;
        TreeNode* probe = root;
        while(probe){
            dl++;
            probe = probe->left;
        }
        while(probe){
            dr++;
            probe = probe->right;
        }
        if(dr==dl){
            return pow(2, dl)-1;//是满二叉树，直接用指数求
        }else{
            int a = countNodes(root->left);
            int b = countNodes(root->right);
            return a+b+1;
        }
    }
};
```
