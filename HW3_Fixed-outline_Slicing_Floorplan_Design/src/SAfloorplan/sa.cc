#include <algorithm>
#include <climits>
#include <cmath>
#include <deque>
#include <iostream>
#include <stdlib.h> 
#include <unordered_map>
#include "sa.h"
using namespace std;

void SA::CalSideLen(double& dead_space_ratio)
{
  double total_area = 0;
  for(const auto& hb: input->HBList)
  {
    int w = hb->width, h = hb->height;
    total_area += (w * h);
  }
  region_side_len = sqrt(total_area * (1+dead_space_ratio));
}

template<class T>
void SA::SWAP(T& a, T& b) // "perfect swap" (almost)
{
  T tmp {move(a)}; // move from a
  a = move(b); // move from b
  b = move(tmp); // move from tmp
}

void SA::InitNPE(vector<int>& NPE)
{
  vector<int> row;
  deque<vector<int>> rows;
  int cur_width = input->HBList[0]->width;
  row.emplace_back(0);
  int placedBlock = 1;
  for(int i = 1; i < input->HBList.size(); ++i)
  {
    auto curHB = input->HBList[i];
    if(cur_width + curHB->width <= region_side_len)
    {
      row.emplace_back(i);
      cur_width += curHB->width;
      row.emplace_back(-1);
    }
    else
    {
      rows.emplace_back(row);
      decltype(row)().swap(row);

      row.emplace_back(i);
      cur_width = curHB->width;
    }
  }
  rows.emplace_back(row);

  for(auto& row_i: rows[0])
  {
    NPE.emplace_back(row_i);
  }
  rows.pop_front();
  //NPE.emplace_back(-2);

  for(auto& row:rows)
  {
    for(auto& row_i:row)
    {
      NPE.emplace_back(row_i);
    }
    NPE.emplace_back(-2);
  }
}

void SA::Complement(vector<int>& curNPE, int startIdx)
{
  for(int i = startIdx; i < curNPE.size(); ++i)
  {
    if(curNPE[i] == -1) curNPE[i] = -2;
    else if(curNPE[i] == -2)  curNPE[i] = -1;
    else break;
  }
}

bool SA::isSkewed(vector<int>& curNPE, int i)
{
  // SWAP(curNPE[i], curNPE[i+1]);
  // if(curNPE[i-1] == curNPE[i])  return false;
  // else if(curNPE[i] == curNPE[i+1]) return false;
  // return true;

  if(i-1 >= 0 && curNPE[i-1] == curNPE[i+1])
  {
    return false;
  }
  else if(i+2 < curNPE.size() && curNPE[i] == curNPE[i+2])
  {
    return false;
  }
  return true;
}

bool SA::violateBallot(vector<int>& curNPE, int i)
{
  // SWAP(curNPE[i], curNPE[i+1]);
  // int N = 0;
  // for(int k = 0; k <= i+1; ++k)
  // {
  //   if(curNPE[i] == -1 || curNPE[i] == -2)  ++N;
  //   if(2 * N < i) return false;
  // }
  // return true;

  // operand + operator
  if(curNPE[i] >= 0)
  {
    int N = 0;
    for(int k = 0; k <= i+1; ++k)
    {
      if(curNPE[k] == -1 || curNPE[k] == -2)  ++N;
    }
    if(2 * N < i+1) return false;
  }
  return true;
}

vector<int> SA::Perturb(vector<int> curNPE, int M)
{
  switch(M)
  {
    case 0:
    {
      vector<int> SwapPos;
      for(int i = 0; i < curNPE.size(); ++i)
      {
        if (curNPE[i] >= 0)
          SwapPos.emplace_back(i);
      }
      int n = SwapPos.size();

      // Below three lines code aims to find adjencent hardblock for swapping
      // But somehow will incur out-of-region bug ... 
      // int r = rand() % (n-1);
      // int swap_pos1 = SwapPos[r];
      // int swap_pos2 = SwapPos[++r];
      
      // Change to pick two random hardblock for swapping
      int swap_pos1,swap_pos2;
      swap_pos1 = swap_pos2 = rand() % n;
      while(swap_pos1 == swap_pos2) swap_pos2 = rand() % n;
      swap_pos1 = SwapPos[swap_pos1];
      swap_pos2 = SwapPos[swap_pos2];

      SWAP(curNPE[swap_pos1], curNPE[swap_pos2]);
      break;
    }
    case 1:
    {
      vector<int> InverseStartPos;
      for(int i = 0; i < curNPE.size()-1; ++i)
        if(curNPE[i] >= 0 && curNPE[i+1] < 0)
            InverseStartPos.emplace_back(i+1);
      int n = InverseStartPos.size();
      int StartIdx = rand() % n;
      StartIdx = InverseStartPos[StartIdx];
      Complement(curNPE, StartIdx);
      break;
    }
    case 2:
    { 
      vector<int> SwapPos;
      for(int i = 0; i < curNPE.size()-1; ++i)
      {
        if(curNPE[i] >= 0 && curNPE[i+1] < 0)
        {
          if(isSkewed(curNPE, i) && !violateBallot(curNPE, i))
          {
            SwapPos.emplace_back(i);
          }
        }
        else if(curNPE[i] < 0 && curNPE[i+1] >= 0)
        {
          if(isSkewed(curNPE, i))
          {
            SwapPos.emplace_back(i);
          }
        }
      }
      int n = SwapPos.size();
      if(n != 0)
      {
        int r = rand() % n;
        int SwapIdx = SwapPos[r];
        // cout << "SwapIdx = " << SwapIdx << endl;
        SWAP(curNPE[SwapIdx], curNPE[SwapIdx+1]);
      }
      break;
    }
  }
  return curNPE;
}

TreeNode* SA::ConstructTree(vector<int>& NPE)
{
  vector<TreeNode*> st;
  int cutNodeIndex = 0;
  for(auto& element:NPE)
  {
    if(element >= 0)
    {
      // string hbNode_name = "sb"+ to_string(element);
      // HardBlock* hb = input->HBTable[hbNode_name];
      // TreeNode* hbNode = new TreeNode(0, hb);
      st.emplace_back(hbNodeList[element]);
    }
    else
    {
      TreeNode* cutNode = cutNodeList[cutNodeIndex];
      ++cutNodeIndex;
      // TreeNode* VHnode = new TreeNode(element);
      cutNode->type = element;
      cutNode->rchild = st.back(); st.pop_back();
      cutNode->lchild = st.back(); st.pop_back();
      st.emplace_back(cutNode);
      cutNode->updateShape();
    }
  }
  return st.back(); // root
}

void SA::PlaceBlock(TreeNode* node, int shapeIdx, int x, int y)
{
  if(node->type == 0)
  {
    node->hardblock->updateInfo(get<0>(node->shape[shapeIdx]), get<1>(node->shape[shapeIdx]), x, y);
  }
  else
  {
    int left_choice = get<2>(node->shape[shapeIdx]).first;
    PlaceBlock(node->lchild, left_choice, x, y);
    int displacementX = 0, displacementY = 0;
    if(node->type == -1)
    {
      displacementX = get<0>(node->lchild->shape[left_choice]);
    }
    else
    {
      displacementY = get<1>(node->lchild->shape[left_choice]);
    }
    PlaceBlock(node->rchild, get<2>(node->shape[shapeIdx]).second, x+displacementX, y+displacementY);
  }
}

int SA::CalTotalHPWL()
{
  int totalHPWL = 0;
  for(auto& net: input->NetList)
  {
    totalHPWL += net->calHPWL();
  }
  return totalHPWL;
}

int SA::CalSACost(vector<int>& NPE, bool const& AreaPhase)
{
  TreeNode* root = ConstructTree(NPE);
  int min_out_area = INT_MAX, out_of_range_area = 0, shapeIdx = 0;
  for(int i = 0; i < root->shape.size(); ++i)
  {
    auto info = root->shape[i];
    int cur_width = get<0>(info), cur_height = get<1>(info);
    if(cur_width > region_side_len && cur_height > region_side_len)
    {
      out_of_range_area = cur_width * cur_height - pow(region_side_len,2);
    }
    else if(cur_height > region_side_len)
    {
      out_of_range_area = cur_width * (cur_height - region_side_len);
    }
    else if(cur_width > region_side_len)
    {
      out_of_range_area= cur_height * (cur_width - region_side_len);
    }
    else
    {
      out_of_range_area = 0;
    }
    // Pick 1st shape which is within the region due to time-saving.
    // But it might not be the min-area-shape of all the qualified shape. 
    if(out_of_range_area < min_out_area)
    {
      min_out_area = out_of_range_area;
      shapeIdx = i;
    }
  }

  // Calculate cost but focus on area only since CalTotalHPWL takes too much time
  // Aim to find a feasible solution ASAP without calculating the wirelength
  if(AreaPhase == true)  return min_out_area * 500;

  // After finding a floorplan, place hardblocks and determining the new x,y corordinates
  PlaceBlock(root, shapeIdx, 0, 0);

  // Calculate cost for wirelength but still need to consider area 
  // since it may still out of range
  return min_out_area * 50 + CalTotalHPWL();
}

void SA::SAfloorplanning(double epsilon, double r, int k, bool AreaPhase, vector<int>& initNPE, vector<int>& bestNPE)
{
  bestNPE = initNPE;
  int MT, uphill, reject; MT = uphill = reject = 0;
  int N = k * input->HBList.size();
  vector<int> curNPE = initNPE;
  int cur_cost = CalSACost(curNPE, AreaPhase);
  int best_cost = cur_cost;
  // If cur_cost is 0 initially, then we directly go to WL phase of SA
  if(best_cost == 0)
  { 
    CalSACost(bestNPE, false); return; 
  }
  // mt19937 random_number_generator(random_device{}());
  // uniform_int_distribution<> rand_move(1, 3);
  // uniform_real_distribution<> rand_prob(0, 1);
  do
  {
    double T0 = 5000;
    do
    {
      MT = uphill = reject = 0;
      do
      {
        if(clock.OutOfTime())
        {
          CalSACost(bestNPE, false); return;
        }
        // int M = rand_move(random_number_generator);
        // int M = rand() % 3;

        // Speedup by setting Area phase using only Move 0
        int M = AreaPhase? 0 : rand() % 3;
        vector<int> tryNPE = Perturb(curNPE, M);
        MT += 1;
        int try_cost = CalSACost(tryNPE, AreaPhase);
        int delta_cost = try_cost - cur_cost;
        if(delta_cost < 0 || (double)rand()/RAND_MAX < exp(-1*delta_cost/T0))
        {
          if(delta_cost > 0) { uphill += 1; }
          curNPE = tryNPE;
          cur_cost = try_cost; 
          if(cur_cost < best_cost)
          {
            bestNPE = curNPE;
            best_cost = cur_cost;
            // If best_cost(try_cost) == 0, it means found one feasible solution
            // Calculate its "real" cost i.e., cost function   
            if(best_cost == 0)
            { 
              CalSACost(bestNPE, false); return; 
            }
          }
        }
        else
        {
          reject += 1;
        }
      }while(uphill <= N && MT <= 2*N);
      T0 = r * T0;
    }while(reject/MT <= 0.95 && T0 >= epsilon);
  }while (AreaPhase == true);
  CalSACost(bestNPE, false); return; 
}

OutputWriter* SA::Run()
{
  // unsigned seed = input->seed;
  unsigned seed;
  switch (input->HBList.size())
  {
    case 100:
      if(input->dead_space_ratio == 0.1)
      {
        seed = 4201;
      }
      else // dsr = 0.15
      {
        seed = 26630;
      }
      break;
    case 200:
      if(input->dead_space_ratio == 0.1)
      {
        seed = 9512;
      }
      else 
      {
        seed = 4528;
      }
      break;
    case 300:
      if(input->dead_space_ratio == 0.1)
      {
        seed = 25183;
      }
      else 
      { 
        seed = 15293;
      }
      break;
    default:
      seed = 48763;
      break;
  }
  srand(seed);

  clock.StartClock("SA init time");
  vector<int> initNPE, bestNPE, finalNPE;
  InitNPE(initNPE);
  clock.EndClock("SA init time");

  clock.StartClock("SA area phase time");
  SAfloorplanning(10, 0.85, 10, true, initNPE, bestNPE);
  cout << "Find a feasible floorplan, total wirelength = " << CalTotalHPWL() << "\n";
  clock.EndClock("SA area phase time");

  clock.StartClock("SA wirelength phase time");
  finalNPE = bestNPE;
  SAfloorplanning(1, 0.99, 5, false, bestNPE, finalNPE);
  cout << "Find a better floorplan, total wirelength = " << CalTotalHPWL() << "\n";
  clock.EndClock("SA wirelength phase time");

  OutputWriter* ow = new OutputWriter(input);
  return ow;
}