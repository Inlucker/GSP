select *
from logs
where command = "EditUndo"
group by session_id;

select command, (row_number() over (partition by same))-1 as cmd_id
from
(
    select 1 as same, command
    from logs
    group by command
);

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
select id, session_id, int_time, logs.command, cmds_ids.cmd_id
from logs left join cmds_ids on (cmds_ids.command = logs.command)
order by id;

select command, count(command)
from logs
group by command;

select * from logs where session_id = 965;

select count(session_id) from (select distinct session_id from logs);

SELECT * FROM logs WHERE command LIKE "%1%" OR command LIKE "%2%" OR command LIKE "%3%" OR command LIKE "%4%" OR command LIKE "%5%" OR command LIKE "%6%" OR command LIKE "%7%" OR command LIKE "%8%" OR command LIKE "%9%" OR command LIKE "%0%" group by command

UPDATE logs SET cmd_id=573 WHERE id=1;

ALTER TABLE logs ADD cmd_id INTEGER;

ALTER TABLE logs DROP COLUMN session_id;

DELETE FROM logs WHERE id > 27850;

DELETE FROM logs WHERE session_id == 948;

select sum(c) from
(
    select session_id, c from
    (
        select session_id, count(session_id) as c
        from logs
        group by session_id
        order by c DESC
    )
    where c > 100
)

DELETE FROM logs WHERE session_id in
(
select session_id from
    (
        select session_id, count(session_id) as c
        from logs
        group by session_id
        order by c DESC
    )
    where c > 100
);