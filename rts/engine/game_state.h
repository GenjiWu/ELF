#pragma once
#include <iostream>
#include "elf/ai.h"
#include "elf/game_base.h"
#include "game_env.h"
#include "game_options.h"
#include "game_action.h"

class RTSState {
public:
    RTSState();

    bool Prepare(const RTSGameOptions &options, ostream *output = nullptr);

    void Save(string *s) const {
        serializer::saver saver(true);
        _env.SaveSnapshot(saver);
        _cmd_receiver.SaveCmdReceiver(saver);
        *s = saver.get_str();
    }

    void Load(const string &s) {
        serializer::loader loader(true);
        loader.set_str(s);
        _env.LoadSnapshot(loader);
        _cmd_receiver.LoadCmdReceiver(loader);
    }

    // Copy construct.
    RTSState &operator=(const RTSState &s) {
        string str;
        s.Save(&str);
        Load(str);
        return *this;
    }

    void LoadSnapshot(const string &filename, bool binary) {
        serializer::loader loader(binary);
        if (! loader.read_from_file(filename)) {
            throw std::range_error("Cannot read from " + filename);
        }
        _env.LoadSnapshot(loader);
        _cmd_receiver.LoadCmdReceiver(loader);
    }

    void SaveSnapshot(const string &filename, bool binary) const {
        serializer::saver saver(binary);
        _env.SaveSnapshot(saver);
        _cmd_receiver.SaveCmdReceiver(saver);
        if (! saver.write_to_file(filename)) {
            throw std::range_error("Cannot write to " + filename);
        }
    }

    int MoveToTick(const std::vector<Tick> &snapshots, float percent) const;

    const GameEnv &env() const { return _env; }
    const CmdReceiver &receiver() const { return _cmd_receiver; }

    Tick GetTick() const { return _cmd_receiver.GetTick(); }

    void SetGlobalStats(GlobalStats *stats) {
        _cmd_receiver.GetGameStats().SetGlobalStats(stats);
    }
    void SetVerbose(bool verbose) { _verbose = verbose; }
    void SetReplayPrefix(const std::string &prefix) { _save_replay_prefix = prefix; }

    // Function used in GameLoop
    virtual bool Init() { return true; }
    virtual void PreAct() { }
    virtual void IncTick() { _cmd_receiver.IncTick(); }

    virtual elf::GameResult PostAct();
    virtual bool forward(RTSAction &);
    virtual void Finalize();

    virtual bool Reset();

    virtual void OnAddPlayer(const std::string &name, int player_id);
    virtual void OnRemovePlayer(int player_id);

private:
    GameEnv _env;
    CmdReceiver _cmd_receiver;

    bool _verbose = false;
    std::string _save_replay_prefix;
    Tick _max_tick = 30000;
};


