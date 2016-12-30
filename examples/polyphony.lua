ps = {}

function get_data()
    print("resetting variables")
    ps = ptk_get_pointer("r")
end

instr_offsets = {
-- FM
0,
-- square
5
}

function makenote(chan, voice, note, sample)
    if(note == 0) then
        ptk_ps_eval(ps, string.format("(ps-noteoff %d -1)", voice + instr_offsets[sample + 1]))
    else
        print(string.format("NOTEON: %d %d %d %d", chan, voice, note, sample))
        ptk_ps_eval(ps, string.format("(ps-argset %d 0 %d)", voice + instr_offsets[sample + 1], note))
        ptk_ps_eval(ps, string.format("(ps-turnon %d -1)", voice + instr_offsets[sample + 1]))
    end
end

function stop()
    print("stopping...")
    ptk_ps_eval(ps, "(ps-clear-events)")
end

ptk_set_note_callback(makenote)
ptk_set_stop_callback(stop)
ptk_toggle_note_callback(0)
ptk_toggle_note_callback(1)


ptk_compile_sporth([[
(
_in 2 zeros
0 0 8 _in _out 'test.scm' _r ref ps
0 7 _out ftsum dup
#_L get _R get
)
]])

get_data()
ptk_set_recompile_callback(get_data)
