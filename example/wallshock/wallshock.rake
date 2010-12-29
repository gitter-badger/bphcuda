cc = Bphcuda.make_default_compiler
figdir = Thrusting.get_figure_dir(File.dirname __FILE__)
FileUtils.mkdir_p(figdir)
p figdir

# s=-3, 100 causes too big max that scales focusing numbers too small
ss = [-3, -2, -1, 0, 2, 100]
["wallshock"].each do |dir|
  # Thrusting.make_compile_task(cc, "release", dir)
  # Thrusting.make_run_task_1(ss, dir)
end

def make_wallshock_graph(plot, s, dev)
  n_cell = 1000
  n_particle_per_cell = 100
  x_cell = 1.0 / n_cell;
  xs = (0...n_cell).map { |i| i * x_cell + 0.5 * x_cell }
  ys = File.read("#{Thrusting.get_data_dir("wallshock", dev)}/#{s}.dat").split("\n").map { |x| x.to_f / n_particle_per_cell }

  plot.data << Gnuplot::DataSet.new( [xs, ys] ) do |ds|
    ds.title = "s=#{s}"
    ds.with = "lines"
  end 
end

namespace :plot do
  task :wallshock do
    Gnuplot.open do |gp|
      Gnuplot::Plot.new(gp) do |plot|
        plot.title "wall shock problem"
        plot.xlabel "x"
        plot.ylabel "density"
        plot.xrange "[0:0.5]"
        plot.yrange "[0:8.0]"
        plot.terminal "jpeg"
        plot.output "#{figdir}/wallshock.jpeg"
        ss.each do |s|
          make_wallshock_graph(plot, s, "host")
        end
      end
    end
  end
end