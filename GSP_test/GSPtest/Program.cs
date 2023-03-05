using System;
using System.Collections.Generic;

namespace GSPtest
{
  class Program
  {
    static void Main(string[] args)
    {
      //Testing Join
      /*Sequence s1 = new Sequence(new List<List<int>> { new List<int> { 1, 2 },
                                                       new List<int> { 3 } });
      Sequence s2 = new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 2 } ),
                                                       new ItemGroup( new List<int> { 3, 4 } ) });
      Sequence s3 = new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 2 } ),
                                                       new ItemGroup( new List<int> { 3 } ),
                                                       new ItemGroup( new List<int> { 5 } )});
      Console.WriteLine(Sequence.IsJoinable(s1, s2));
      Console.WriteLine(Sequence.IsJoinable(s1, s3));

      Sequence s12 = Sequence.Join(s1, s2);
      Sequence s13 = Sequence.Join(s1, s3);*/

      //Testing generate
      /*List<Sequence> cur_freq_seqs = new List<Sequence>(0);
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 1, 2 } ),
                                                           new ItemGroup( new List<int> { 3 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 1, 2 } ),
                                                           new ItemGroup( new List<int> { 4 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 1 } ),
                                                           new ItemGroup( new List<int> { 3, 4 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 1, 3 } ),
                                                           new ItemGroup( new List<int> { 5 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 2 } ),
                                                           new ItemGroup( new List<int> { 3, 4 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 2 } ),
                                                           new ItemGroup( new List<int> { 3 } ),
                                                           new ItemGroup( new List<int> { 5 } )}));*/

      GSP gsp = new GSP();
      //gsp.Test1();

      //GSP gsp = new GSP();
      //gsp.Test2();
      //gsp.Test3();

      //LogReader log_reader = new LogReader();
      //log_reader.ReadLog("C:\\Users\\arsen\\AppData\\Roaming\\Nanosoft\\nanoCAD x64 22.0\\nnmsstcs\\2022-09.log");

      //gsp.Test4();
      gsp.Test5();

      Console.WriteLine("Press any key to exit");
      Console.ReadLine();
    }
  }
}
