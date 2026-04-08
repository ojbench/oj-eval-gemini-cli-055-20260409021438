#ifndef SRC_HPP
#define SRC_HPP

#include <iostream>
#include <algorithm>
#include "base.hpp"

using namespace std;

struct date {
    int year, month, day;
    date() = default;
    date(int y, int m, int d) : year(y), month(m), day(d) {}
    
    friend istream& operator>>(istream& is, date& d) {
        is >> d.year >> d.month >> d.day;
        return is;
    }
    
    bool operator<(const date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    
    bool operator<=(const date& other) const {
        return !(other < *this);
    }
    
    bool operator==(const date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
};

class mail : public object {
protected:
    string postmark;
    date send_date;
    date arrive_date;

public:
    mail() = default;

    mail(string _contain_, string _postmark_, date send_d, date arrive_d) : object(_contain_) {
        postmark = _postmark_;
        send_date = send_d;
        arrive_date = arrive_d;
    }

    virtual string send_status(int y, int m, int d) override {
        return "not send";
    }

    virtual string type() override {
        return "no type";
    }

    virtual void print() override {
        object::print();
        cout << "[mail] postmark: " << postmark << endl;
    }

    virtual void copy(object *o) override {
        mail* m = reinterpret_cast<mail*>(o);
        contain = m->contain;
        postmark = m->postmark;
        send_date = m->send_date;
        arrive_date = m->arrive_date;
    }
    
    virtual ~mail() {}
};

class air_mail : public mail {
protected:
    string airlines;
    date take_off_date;
    date land_date;

public:
    air_mail() = default;

    air_mail(string _contain_, string _postmark_, date send_d, date arrive_d, date take_off, date land, string _airline) 
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        take_off_date = take_off;
        land_date = land;
        airlines = _airline;
    }

    virtual string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        else if (ask_date < take_off_date)
            return "wait in airport";
        else if (ask_date < land_date)
            return "in flight";
        else if (ask_date < arrive_date)
            return "already land";
        else
            return "already arrive";
    }

    virtual string type() override {
        return "air";
    }

    virtual void print() override {
        mail::print();
        cout << "[air] airlines: " << airlines << endl;
    }

    virtual void copy(object *o) override {
        air_mail* m = reinterpret_cast<air_mail*>(o);
        contain = m->contain;
        postmark = m->postmark;
        send_date = m->send_date;
        arrive_date = m->arrive_date;
        take_off_date = m->take_off_date;
        land_date = m->land_date;
        airlines = m->airlines;
    }
    
    virtual ~air_mail() {}
};

class train_mail : public mail {
protected:
    string *station_name;
    date *station_time;
    int len;

public:
    train_mail() : station_name(nullptr), station_time(nullptr), len(0) {}

    train_mail(string _contain_, string _postmark_, date send_d, date arrive_d, string *sname, date *stime, int station_num)
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        len = station_num;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = sname[i];
                station_time[i] = stime[i];
            }
        } else {
            station_name = nullptr;
            station_time = nullptr;
        }
    }

    virtual string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        if (arrive_date <= ask_date)
            return "already arrive";
            
        if (len == 0) {
            return "wait in departure station";
        }
        
        if (ask_date < station_time[0])
            return "wait in departure station";
            
        int l = 0, r = len - 1;
        while (l <= r) {
            int mid = l + (r - l) / 2;
            if (station_time[mid] == ask_date)
                return "at " + station_name[mid];
            else if (station_time[mid] < ask_date)
                l = mid + 1;
            else
                r = mid - 1;
        }
        
        if (l < len) {
            return "between " + station_name[l - 1] + " and " + station_name[l];
        } else {
            return "wait in terminus station";
        }
    }

    virtual string type() override {
        return "train";
    }

    virtual void print() override {
        mail::print();
        cout << "[train] station_num: " << len << endl;
    }

    virtual void copy(object *o) override {
        if (this == o) return;
        train_mail* m = reinterpret_cast<train_mail*>(o);
        contain = m->contain;
        postmark = m->postmark;
        send_date = m->send_date;
        arrive_date = m->arrive_date;
        
        if (station_name) delete[] station_name;
        if (station_time) delete[] station_time;
        
        len = m->len;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = m->station_name[i];
                station_time[i] = m->station_time[i];
            }
        } else {
            station_name = nullptr;
            station_time = nullptr;
        }
    }
    
    virtual ~train_mail() {
        if (station_name) delete[] station_name;
        if (station_time) delete[] station_time;
    }
};

class car_mail : public mail {
protected:
    int total_mile;
    string driver;

public:
    car_mail() = default;

    car_mail(string _contain_, string _postmark_, date send_d, date arrive_d, int mile, string _driver)
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        total_mile = mile;
        driver = _driver;
    }

    virtual string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        else if (ask_date < arrive_date) {
            int used_days = (ask_date.year - send_date.year) * 360 + (ask_date.month - send_date.month) * 30 + (ask_date.day - send_date.day);
            int total_days = (arrive_date.year - send_date.year) * 360 + (arrive_date.month - send_date.month) * 30 + (arrive_date.day - send_date.day);
            double current_mile = (double)used_days / total_days * total_mile;
            return "at " + to_string(current_mile);
        } else {
            return "already arrive";
        }
    }

    virtual string type() override {
        return "car";
    }

    virtual void print() override {
        mail::print();
        cout << "[car] driver_name: " << driver << endl;
    }

    virtual void copy(object *o) override {
        car_mail* m = reinterpret_cast<car_mail*>(o);
        contain = m->contain;
        postmark = m->postmark;
        send_date = m->send_date;
        arrive_date = m->arrive_date;
        total_mile = m->total_mile;
        driver = m->driver;
    }
    
    virtual ~car_mail() {}
};

void obj_swap(object *&x, object *&y) {
    object *tmp = x;
    x = y;
    y = tmp;
}

#endif
