#include "z3++.h"
#include "include/Solver.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>

using namespace std;
using namespace z3;

#define EQ(exp, i) (atmost((exp), (i)) && atleast((exp), (i)))

const int dx[] = {-1, 0, 1, 0, 0};
const int dy[] = { 0,-1, 0, 1, 0};

Solver::Solver(Architecture& arch, z3::context& c): arch_(arch), ctx_(c), solver_(c), no_of_actions_(c), optimize_handle_(1), model_(c) {
    // read width, height, ...
    width_limit_ = arch_.width_limit_;
    height_limit_ = arch_.height_limit_;
    time_limit_ = arch.time_limit_;
}

bool Solver::solve(){
    try {
        for(int width = 2; width <= width_limit_; width++){
            for(int height = 2; height <= height_limit_; height++){
                for(int time = 2; time <= time_limit_; time++){
                    init(width, height, time);
                    add_constraints();

                    result_ = solver_.check();
                    if(result_ == sat){
                        model_ = solver_.get_model();
                        cout << "Sat - (w=" << width << ", h=" << height << ", t=" << time << ")" << endl;
                        //cout << "Printing sat constraints to file" << endl;

                        return true;
                    }else{
                        cout << "Unsat - (w=" << width << ", h=" << height << ", t=" << time << ")" << endl;
                    }
                }
            }
        }    
    } catch(z3::exception e){
        cerr << e.msg() << endl;
        return true;
    }
    return false;
}

bool Solver::solve(int width, int height, int time){
    try {
        init(width, height, time);
        add_constraints();

        result_ = solver_.check();
        if(result_ == sat){
            model_ = solver_.get_model();
            cout << "Sat - (w=" << width << ", h=" << height << ", t=" << time << ")" << endl;
            // cout << "Printing sat constraints to file" << endl;

            return true;
        }else{
            cout << "Unsat - (w=" << width << ", h=" << height << ", t=" << time << ")" << endl;
            return false;
        }
    } catch(z3::exception e){
        cerr << e.msg() << endl;
        return true;
    }
    return false;
}

void Solver::print_solution(ostream& out){ 
    if(result_ == unsat){
        return;
    }

    expr TRUE = ctx_.bool_val(true);
    // TODO: fix this 
    out << "Dispenser position(s): " << endl;
    for(auto pair: arch_.modules_){
        Module module = pair.second;
        if(module.type_ == DISPENSER){
            for(int p = 0; p < perimeter_cur_; p++){
                if(eq(model_.eval(dispenser_[p][module.id_]), TRUE)){
                    out << module.label_ << " at " << p << endl;
                }
            }
        }
    }
    out << endl;

    out << "Sink position(s): " << endl;
    for(auto pair: arch_.modules_){
        Module module = pair.second;
        if(module.type_ == SINK){
            for(int p = 0; p < perimeter_cur_; p++){
                if(eq(model_.eval(sink_[p][module.id_]), TRUE)){
                    out << module.label_ << " at " << p << endl;
                }
            }
        }
    }
    out << endl;

    out << "Detector position(s): " << endl;
    for(auto pair: arch_.modules_){
        Module module = pair.second;
        if(module.type_ == DETECTOR){
            for(int x = 0; x < width_cur_; x++){
                for(int y = 0; y < height_cur_; y++){
                    if(eq(model_.eval(detector_[x][y][module.id_]), TRUE)){
                        out << module.label_ << " at (" << x << ", " << y << ")" << endl;
                    }
                }
            }
        }
    }
    out << endl;

    for(int t = 0; t <= time_cur_; t++){
        out << "time = " << t << endl;
        for(int y = 0; y < height_cur_; y++){
            for(int x = 0; x < width_cur_; x++){
                bool flag = false;
                for(auto module: arch_.nodes_){
                    if(module.type_ == DETECTOR){
                        int detector_id = arch_.modules_[module.label_].id_;
                        if(eq(model_.eval(detector_[x][y][detector_id]), TRUE) && eq(model_.eval(detecting_[t][module.id_]), TRUE)){
                            out << "d ";
                            flag = true;
                            break;
                        }
                    }else if(module.type_ == MIXER){
                        if(eq(model_.eval(mixing_[t][x][y][module.id_]), TRUE)){
                            out << "m ";
                            flag = true;
                            break;
                        }
                    }
                }
                for(int i = 0; i < no_of_edges_; i++){
                    if(eq(model_.eval(c_[t][x][y][i]), TRUE)){
                        out << i << ' ';
                        flag = true;
                        break;
                    }
                }
                if(!flag)
                    out << "e ";
            }
            out << endl;
        }
        out << endl;
    }
}

void Solver::save_solution(const string& filename){
    ofstream out(filename);
    if(out.is_open()){
        print_solution(out);
    }
    out.close();
}

vector<vector<vector<pair<int, int>>>> Solver::get_grid(){
    if(result_ == unsat){
        return vector<vector<vector<pair<int, int>>>>();
    }
    expr TRUE = ctx_.bool_val(true);
    vector<vector<vector<pair<int, int>>>> res;
    res.resize(time_cur_+1);
    for(int t = 0; t <= time_cur_; t++){
        res[t].resize(height_cur_);
        for(int y = 0; y < height_cur_; y++){
            for(int x = 0; x < width_cur_; x++){
                bool flag = false;
                for(auto module: arch_.nodes_){
                    if(module.type_ == DETECTOR){
                        int detector_id = arch_.modules_[module.label_].id_;
                        if(eq(model_.eval(detector_[x][y][detector_id]), TRUE) && eq(model_.eval(detecting_[t][module.id_]), TRUE)){
                            res[t][y].emplace_back(-1, 0);
                            flag = true;
                            break;
                        }
                    }else if(module.type_ == MIXER){
                        if(eq(model_.eval(mixing_[t][x][y][module.id_]), TRUE)){
                            res[t][y].emplace_back(-2, 0);
                            flag = true;
                            break;
                        }
                    }
                }
                for(int i = 0; i < no_of_edges_; i++){
                    if(eq(model_.eval(c_[t][x][y][i]), TRUE)){
                        res[t][y].emplace_back(i, arch_.edge_weights_[i]);
                        flag = true;
                        break;
                    }
                }
                if(!flag)
                    res[t][y].emplace_back(-3, 0);
            }
        }
    }
    return res;
}

std::vector<int> Solver::get_sink_dispenser_pos(){
    if(result_ == unsat){
        return vector<int>();
    }
    expr TRUE = ctx_.bool_val(true);
    vector<int> res;
    for(int p = 0; p < perimeter_cur_; p++){
        int num;
        bool flag = false;
        for(auto pair: arch_.modules_){
            Module m = pair.second;
            if(m.type_ == DISPENSER){
                if(eq(model_.eval(dispenser_[p][m.id_]), TRUE)){
                    num = 2;
                    flag = true;
                    break;
                }
            }else if(m.type_ == SINK){
                if(eq(model_.eval(sink_[p][m.id_]), TRUE)){
                    num = 1;
                    flag = true;
                }
            }
        }
        if(!flag){
            num = 0;
        }
        res.push_back(num);
    }

    return res;
}

std::vector<std::vector<std::pair<bool, std::string>>> Solver::get_detector_pos(){
    if(result_ == unsat){
        return vector<vector<pair<bool, string>>>();
    }
    expr TRUE = ctx_.bool_val(true);
    vector<vector<pair<bool, string>>> res;
    res.resize(height_cur_);
    for(int y = 0; y < height_cur_; y++){
        res[y].resize(width_cur_, make_pair(false, ""));
    }

    for(auto pair: arch_.modules_){
        Module m = pair.second;
        if(m.type_ == DETECTOR){
            for(int x = 0; x < width_cur_; x++){
                for(int y = 0; y < height_cur_; y++){
                    if(eq(model_.eval(detector_[x][y][m.id_]), TRUE)){
                        res[y][x] = make_pair(true, m.label_);
                    }
                }
            }
        }
    }
    return res;
}


void Solver::init(int width, int height, int time){
    // init variables
    c_.clear();
    detecting_.clear();
    mixing_.clear();
    detector_.clear();
    dispenser_.clear();
    sink_.clear();
    solver_ = optimize(ctx_);

    width_cur_ = width;
    height_cur_ = height;
    perimeter_cur_ = (width + height) * 2;
    time_cur_ = time;

    no_of_modules_ = arch_.modules_.size();
    no_of_nodes_ = arch_.nodes_.size();
    no_of_edges_ = arch_.edges_.size();

    // count otal droplet/mixer appearance
    expr_vector counter(ctx_);
    // constants
    expr zero = ctx_.int_val(0);
    expr one = ctx_.int_val(1);
    // c^t_(x,y,id)
    c_.resize(time+1);
    for(int t = 1; t <= time; t++){
        c_[t].resize(width);
        for(int w = 0; w < width; w++){
            c_[t][w].resize(height);
            for(int h = 0; h < height; h++){
                // see definition of id
                for(int id = 0; id < no_of_edges_; id++){
                    char name[50];
                    sprintf(name, "c^%d_(%d,%d,%d)", t, w, h, id); // c^t_(x,y,id)
                    c_[t][w][h].push_back(ctx_.bool_const(name));

                    expr e = ite(c_[t][w][h][id], one, zero);
                    counter.push_back(e);
                }
            }
        }
    }
    c_[0].resize(width);
    for(int w = 0; w < width; w++){
        c_[0][w].resize(height);
        for(int h = 0; h < height; h++){
            c_[0][w][h].resize(no_of_edges_, ctx_.bool_val(false));
        }
    }

    // mixing^t_(x,y,id)
    mixing_.resize(time+1);
    for(int t = 0; t <= time; t++){
        mixing_[t].resize(width);
        for(int x = 0; x < width; x++){
            mixing_[t][x].resize(height);
            for(int y = 0; y < height; y++){
                for(int i = 0; i < no_of_nodes_; i++){
                    char name[100];
                    sprintf(name, "mixing^%d_(%d,%d,%d)", t, x, y, i);
                    mixing_[t][x][y].push_back(ctx_.bool_const(name));

                    counter.push_back(ite(mixing_[t][x][y][i], one, zero));
                }
            }
        }
    }
    mixing_[0].resize(width);
    for(int w = 0; w < width; w++){
        mixing_[0][w].resize(height);
        for(int h = 0; h < height; h++){
            mixing_[0][w][h].resize(no_of_nodes_, ctx_.bool_val(false));
        }
    }

    // detecting^t_(l)
    detecting_.resize(time+1);
    detecting_[0].resize(no_of_nodes_, ctx_.bool_val(false));
    for(int t = 1; t <= time; t++){
        for(int i = 0; i < no_of_nodes_; i++){
            char name[50];
            sprintf(name, "detecting^%d_(%d)", t, i);
            detecting_[t].push_back(ctx_.bool_const(name));

            expr e = ite(detecting_[t][i], one, zero);
            counter.push_back(e);
        } 
    }

    // add optimizing condition to solver to reduce total number of steps
    no_of_actions_ = ctx_.int_const("no_of_actions");
    solver_.add(no_of_actions_ == sum(counter));
    optimize_handle_ = solver_.minimize(no_of_actions_);

    // detector_(x,y,l)
    detector_.resize(width);
    for(int w = 0; w < width; w++){
        detector_[w].resize(height);
        for(int h = 0; h < height; h++){
            for(int l = 0; l < no_of_nodes_; l++){ // TODO: check
                char name[50];
                sprintf(name, "detector_(%d,%d,%d)", w, h, l);
                detector_[w][h].push_back(ctx_.bool_const(name));
            }
        }
    }

    // dispenser_(p,l)
    dispenser_.resize(perimeter_cur_);
    for(int p = 0; p < perimeter_cur_; p++){
        for(int l = 0; l < no_of_nodes_; l++){ // TODO: check
            char name[50];
            sprintf(name, "dispenser_(%d,%d)", p, l);
            dispenser_[p].push_back(ctx_.bool_const(name));
        }
    }

    // sink_(p)
    sink_.resize(perimeter_cur_);
    for(int p = 0; p < perimeter_cur_; p++){
        for(int l = 0; l < no_of_nodes_; l++){
            char name[50];
            sprintf(name, "sink_(%d, %d)", p, l);
            sink_[p].push_back(ctx_.bool_const(name));
        }
    }
}

void Solver::add_consistency_constraints(){
    // a cell may not be occupied by more than one droplet or mixer i per time step
    for(int t = 1; t <= time_cur_; t++){
        for(int x = 0; x < width_cur_; x++){
            for(int y = 0; y < height_cur_; y++){
                expr_vector v_tmp(ctx_);
                // mixer or detector node
                for(auto module: arch_.nodes_){
                    if(module.type_ == MIXER){
                        v_tmp.push_back(mixing_[t][x][y][module.id_]);
                    }else if(module.type_ == DETECTOR){
                        v_tmp.push_back(detecting_[t][module.id_]);
                    }
                }

                // droplets
                for(int i = 0; i < no_of_edges_; i++){
                    v_tmp.push_back(c_[t][x][y][i]);
                }
                solver_.add(atmost(v_tmp, 1));
            }
        }
    }

    // each droplet i may occur in at most one cell per time step
    for(int i = 0; i < no_of_edges_; i++){
        for(int t = 1; t <= time_cur_; t++){
            expr_vector v_tmp(ctx_);
            for(int x = 0; x < width_cur_; x++){
                for(int y = 0; y < height_cur_; y++){
                    v_tmp.push_back(c_[t][x][y][i]);
                }
            }
            solver_.add(atmost(v_tmp, 1));
        }
    }

    // in each position p outside of the grid, there may be at most one dispenser (this applies for all types l) or sink
    for(int p = 0; p < perimeter_cur_; p++){
        expr_vector v_tmp(ctx_);
        //v_tmp.push_back(sink_[p]);
        for(auto module: arch_.modules_){
            if(module.second.type_ == DISPENSER){
                v_tmp.push_back(dispenser_[p][module.second.id_]);
            }else if(module.second.type_ == SINK){
                v_tmp.push_back(sink_[p][module.second.id_]); // changed
            }
        }
        solver_.add(atmost(v_tmp, 1));
    }

    // each cell may be occupied by atmost one detector
    for(int x = 0; x < width_cur_; x++){
        for(int y = 0; y < height_cur_; y++){
            expr_vector v_tmp(ctx_);
            for(auto module: arch_.modules_){
                if(module.second.type_ == DETECTOR){
                    v_tmp.push_back(detector_[x][y][module.second.id_]);
                }
            }
            if(!v_tmp.empty()){
                solver_.add(atmost(v_tmp, 1));
            }
        }
    }
}

void Solver::add_placement_constraints(){
    // detectors over all possible cells, one detector for every type l of fluids is placed
    for(auto module: arch_.modules_){
        if(module.second.type_ == DETECTOR){
            expr_vector v_tmp(ctx_);
            for(int x = 0; x < width_cur_; x++){
                for(int y = 0; y < height_cur_; y++){
                    v_tmp.push_back(detector_[x][y][module.second.id_]);
                }
            }
            solver_.add(atleast(v_tmp, 1));
            solver_.add(atmost(v_tmp, 1));
        }
    }

    // dispensers and sinks, desired amount of every type of dispensers and sinks are placed
    for(auto module: arch_.modules_){
        if(module.second.type_ == DISPENSER){
            expr_vector v_tmp(ctx_);
            for(int p = 0; p < perimeter_cur_; p++){
                v_tmp.push_back(dispenser_[p][module.second.id_]);
            }
            solver_.add(atleast(v_tmp, module.second.desired_amount_));
            solver_.add(atmost(v_tmp, module.second.desired_amount_));
        }else if(module.second.type_ == SINK){
            expr_vector v_tmp(ctx_);
            for(int p = 0; p < perimeter_cur_; p++){
                v_tmp.push_back(sink_[p][module.second.id_]);
            }
            solver_.add(atleast(v_tmp, module.second.desired_amount_));
            solver_.add(atmost(v_tmp, module.second.desired_amount_));
        }
    }
}

void Solver::add_movement(){
    for(int i = 0; i < no_of_edges_; i++){
        for(int x = 0; x < width_cur_; x++){
            for(int y = 0; y < height_cur_; y++){
                for(int t = 1; t <= time_cur_; t++){
                    expr_vector vec(ctx_);
                    // move
                    for(int k = 0; k < 5; k++){
                        int xx = x + dx[k];
                        int yy = y + dy[k];
                        if(is_point_inbound(xx, yy)){
                            vec.push_back(c_[t-1][xx][yy][i]);
                        }
                    }

                    // from dispenser
                    int id = arch_.edges_[i].first;
                    if(arch_.nodes_[id].type_ == DISPENSER){ // if the dispenser is of the same type
                        string label = arch_.nodes_[id].label_;
                        int dispenser_id = arch_.modules_[label].id_;
                        if(x == 0){ // (x,y) on left edge
                            vec.push_back(dispenser_[perimeter_cur_ - 1 - y][dispenser_id]);
                        }
                        if(x == width_cur_-1){ // right edge
                            vec.push_back(dispenser_[width_cur_ - 1 + y][dispenser_id]);
                        }
                        if(y == 0){ // top edge
                            vec.push_back(dispenser_[x][dispenser_id]);
                        }
                        if(y == height_cur_-1){ // bottom edge
                            vec.push_back(dispenser_[2*width_cur_ + height_cur_ - 1 - x][dispenser_id]);
                        }
                    }

                    // from mix
                    if(arch_.nodes_[id].type_ == MIXER){
                        int d = arch_.nodes_[id].time_;
                        if(t >= d + 2){
                            string label = arch_.nodes_[id].label_;
                            int mixer_w = arch_.modules_[label].w;
                            int mixer_h = arch_.modules_[label].h;
                            for(int dir = 0; dir < mixer_w*mixer_h; dir++){
                                int x0 = x - dir % mixer_w;
                                int y0 = y - dir / mixer_w;
                                if(is_point_inbound(x0, y0) && is_point_inbound(x+mixer_w-1, y+mixer_h-1)){
                                    expr_vector mix_vec(ctx_);
                                    for(int m = 0; m < no_of_edges_; m++){
                                        if(arch_.edges_[m].second == id){
                                            expr_vector appear_before_mix(ctx_);
                                            for(int ddx = 0; ddx < mixer_w; ddx++){
                                                for(int ddy = 0; ddy < mixer_h; ddy++){
                                                    int x_new = x0 + ddx;
                                                    int y_new = y0 + ddy;
                                                    if(is_point_inbound(x_new, y_new)){
                                                        appear_before_mix.push_back(c_[t-d-1][x_new][y_new][m]);
                                                    }
                                                }
                                            }
                                            expr_vector diappear_on_mix(ctx_);
                                            for(int xx = 0; xx < width_cur_; xx++){
                                                for(int yy = 0; yy < height_cur_; yy++){
                                                    diappear_on_mix.push_back(c_[t-d][xx][yy][m]);
                                                }
                                            }
                                            mix_vec.push_back(mk_or(appear_before_mix));
                                            mix_vec.push_back(!mk_or(diappear_on_mix));
                                        }
                                    }

                                    expr_vector mixing_vec(ctx_);
                                    for(int xx = 0; xx < mixer_w; xx++){
                                        for(int yy = 0; yy < mixer_h; yy++){
                                            for(int t_lag = t-d; t_lag < t; t_lag++){
                                                mixing_vec.push_back(mixing_[t_lag][xx+x0][yy+y0][id]);
                                            }
                                        }
                                    }
                                    mix_vec.push_back(mk_and(mixing_vec));

                                    expr_vector d_vec(ctx_); // all output from the same mixing operation must appear together on the same mixer
                                    for(int m = 0; m < no_of_edges_; m++){
                                        if(arch_.edges_[m].first == id && m != i){ // is output of mixing op
                                            expr_vector appear_at_t(ctx_);
                                            for(int x_new = x0; x_new < x0+mixer_w; x_new++){
                                                for(int y_new = y0; y_new < y0+mixer_h; y_new++){
                                                    appear_at_t.push_back(c_[t][x_new][y_new][m]);
                                                }
                                            }
                                            expr_vector no_before_t(ctx_);
                                            for(int x_new = 0; x_new < width_cur_; x_new++){
                                                for(int y_new = 0; y_new < height_cur_; y_new++){
                                                    no_before_t.push_back(c_[t-1][x_new][y_new][m]);
                                                }
                                            }
                                            d_vec.push_back(mk_or(appear_at_t) && !mk_or(no_before_t));
                                        }
                                    }
                                    mix_vec.push_back(mk_and(d_vec));

                                    vec.push_back(mk_and(mix_vec));
                                }
                            }
                        }
                    }
                    
                    // from detection
                    if(arch_.nodes_[id].type_ == DETECTOR){
                        int d = arch_.nodes_[id].time_;
                        int detector_id = arch_.modules_[arch_.nodes_[id].label_].id_;
                        if(t >= d + 2){
                            for(int m = 0; m < no_of_edges_; m++){
                                if(arch_.edges_[m].second == id){
                                    expr_vector detec_vec(ctx_);

                                    detec_vec.push_back(detector_[x][y][detector_id]);
                                    detec_vec.push_back(c_[t-d-1][x][y][m]);
                                    detec_vec.push_back(!c_[t-d][x][y][m]);
                                    for(int t_lag = t-d; t_lag < t; t_lag++){
                                        detec_vec.push_back(detecting_[t_lag][id]);
                                    }
                                    vec.push_back(mk_and(detec_vec));
                                    break;
                                }
                            }
                        }
                    }

                    if(vec.size() > 0){
                        solver_.add(implies(c_[t][x][y][i], atmost(vec, 1)));
                        solver_.add(implies(c_[t][x][y][i], atleast(vec, 1)));
                    }else{
                        solver_.add(implies(c_[t][x][y][i], ctx_.bool_val(false)));
                    }

                }
            }
        }
    }

    // disappearance
    for(int m = 0; m < no_of_nodes_; m++){
        if(arch_.nodes_[m].type_ == SINK){
            int id_sink = arch_.modules_[arch_.nodes_[m].label_].id_;
            expr_vector mix_vec(ctx_);
            for(int i = 0; i < no_of_edges_; i++){
                if(arch_.edges_[i].second == m){
                    for(int x = 0; x < width_cur_; x++){
                        for(int y = 0; y < height_cur_; y++){
                            for(int t = 2; t <= time_cur_; t++){
                                expr_vector vec(ctx_);
                                // disappear at t;;
                                for(int k = 0; k < 5; k++){
                                    int xx = x + dx[k];
                                    int yy = y + dy[k];
                                    if(is_point_inbound(xx, yy)){
                                        vec.push_back(c_[t][xx][yy][i]);
                                    }
                                }
                                expr d_tmp = not(mk_or(vec));
                                expr disappear = c_[t-1][x][y][i] && d_tmp;

                                expr_vector b_vec(ctx_); // there is a sink at reachable position

                                if(x == 0){ // (x,y) on left edge
                                    b_vec.push_back(sink_[perimeter_cur_ - 1 - y][id_sink]);
                                }
                                if(x == width_cur_-1){ // right edge
                                    b_vec.push_back(sink_[width_cur_ - 1 + y][id_sink]);
                                }
                                if(y == 0){ // top edge
                                    b_vec.push_back(sink_[x][id_sink]);
                                }
                                if(y == height_cur_-1){ // bottom edge
                                    b_vec.push_back(sink_[2*width_cur_ + height_cur_ - 1 - x][id_sink]);
                                }
                                if(b_vec.size() > 0){
                                    solver_.add(implies(disappear, mk_or(b_vec)));
                                }else{
                                    solver_.add(implies(disappear, ctx_.bool_val(false)));
                                }
                            }
                        }
                    }

                    expr_vector disappear_last(ctx_);
                    for(int x = 0; x < width_cur_; x++){
                        for(int y = 0; y < height_cur_; y++){
                            disappear_last.push_back(c_[time_cur_][x][y][i]);
                        }
                    }
                    solver_.add(!mk_or(disappear_last));
                }
            }
        }
    }
}



void Solver::add_objectives(){
    // all droplets have to be present for at least one step
    expr_vector all_droplets_appear_vec(ctx_);
    for(int i = 0; i < no_of_edges_; i++){
        expr_vector v_tmp(ctx_);
        for(int t = 1; t <= time_cur_; t++){
            for(int x = 0; x < width_cur_; x++){
                for(int y = 0; y < height_cur_; y++){
                        v_tmp.push_back(c_[t][x][y][i]);
                }
            }
        }
        all_droplets_appear_vec.push_back(mk_or(v_tmp));
    }
    solver_.add(mk_and(all_droplets_appear_vec));
}

void Solver::add_fluidic_constraints(){
    for(int i = 0; i < no_of_edges_; i++){
        for(int t = 1; t < time_cur_; t++){
            for(int x = 0; x < width_cur_; x++){
                for(int y = 0; y < height_cur_; y++){
                    expr a = c_[t][x][y][i];

                    for(int x_new = x-1; x_new <= x+1; x_new++){
                        for(int y_new = y-1; y_new <= y+1; y_new++){
                            if(is_point_inbound(x_new, y_new)){
                                for(int j = 0; j < no_of_edges_; j++){
                                    if(j == i){
                                        continue;
                                    }
                                    expr a = c_[t][x][y][i] && c_[t][x_new][y_new][j];
                                    // (1): for any droplet^t_i, if there is another droplet nearb at time t 
                                    // they should be mixed together at time t+1
                                    expr_vector c1_vec(ctx_);
                                    for(int xx = 0; xx < width_cur_; xx++){
                                        for(int yy = 0; yy < height_cur_; yy++){
                                            c1_vec.push_back(c_[t+1][xx][yy][i]);
                                            c1_vec.push_back(c_[t+1][xx][yy][j]);
                                        }
                                    }
                                    solver_.add(implies(a, !mk_or(c1_vec)));
                                    
                                    if(t < time_cur_-1){
                                        expr b = c_[t][x][y][i] && c_[t+1][x_new][y_new][j];
                                        // (2): for any droplet^t_i, if there is another droplet nearb at time t+1
                                        // droplet^(t+1)_i mixed with droplet^(t+2)_j
                                        expr_vector c2_vec(ctx_);
                                        for(int xx = 0; xx < width_cur_; xx++){
                                            for(int yy = 0; yy < height_cur_; yy++){
                                                c2_vec.push_back(c_[t+1][xx][yy][i]);
                                                c2_vec.push_back(c_[t+2][xx][yy][j]);
                                            }
                                        }
                                        solver_.add(implies(b, !mk_or(c2_vec)));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Solver::add_constraints(){
    add_consistency_constraints();
    add_placement_constraints();
    add_movement();
    add_objectives();
    add_fluidic_constraints();
}
