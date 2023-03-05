select *
from logs
where command = "EditUndo"
group by session_id;

select command, (row_number() over (partition by same))-1 as cmd_id
from(
    select 1 as same, command
    from logs
    group by command);

with cmds_ids(command, cmd_id) as
(
select command, (row_number() over (partition by same))-1 as cmd_id
    from
    (
        select 1 as same, command
        from logs
        group by command
    )
)
select id, session_id, time, logs.command, cmd_id
from cmds_ids join logs on (cmds_ids.command = logs.command);


select command, count(command)
from logs
group by command;