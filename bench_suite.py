#!/usr/bin/env python3
from argparse import ArgumentParser
from multiprocessing.pool import Pool
from os import makedirs, getcwd, chdir
from os.path import join
from shutil import copy
from subprocess import check_call, check_output
from tempfile import mkdtemp
from multiprocessing import cpu_count


def run_experiment(root, hash, experiment):
    command = "./run_testset.sh {} {} {}".format(join(root, 'bin', 'sitoa_{}'.format(hash)), root, experiment)
    # print(command)
    return check_output(command, shell=True).decode("utf-8").strip()

def main():
    p = ArgumentParser()
    p.add_argument("--tempdir", dest="tmpdir", default="/tmp")
    p.add_argument("--num-commits", dest="ncommits", default=10, type=int)
    p.add_argument("--repetitions", dest="nreps", default=10, type=int)
    p.add_argument("--num-cores", dest="ncores", default=0, type=int)
    p.add_argument("testsets", nargs="+")

    args = p.parse_args()

    tempdir = mkdtemp(dir=args.tmpdir)
    check_call("git archive master games/ testsets/ | tar -x -C {}".format(tempdir), shell=True)
    hashes = check_output("git log --pretty=format:'%h' -n {}".format(args.ncommits), shell=True).decode("utf-8").split()

    my_dir = getcwd()

    makedirs(join(tempdir, 'bin'))

    for h in hashes:
        chdir(my_dir)
        src_dir = join(tempdir, 'src', h)
        makedirs(src_dir)
        check_call("git archive {} | tar -x -C {}".format(h,src_dir), shell=True)
        chdir(src_dir)
        check_call("./make_submission.sh", shell=True)
        copy(join(src_dir, 'subm', 'sitoa'), join(tempdir, 'bin', 'sitoa_{}'.format(h)))

    chdir(my_dir)

    pool = Pool(cpu_count() if args.ncores == 0 else args.ncores)

    experiment_keys = []
    for h in hashes:
        for s in args.testsets:
            experiment_keys.append((tempdir,h,s))

    result = pool.starmap(run_experiment, experiment_keys)

    results = {(a[1],a[2]):b for a,b in zip(experiment_keys, result)}

    # print(results)
    print("{:6}".format(""), end="\t")
    for t in args.testsets:
        print("{:6}".format(t), end="\t")
    print()

    for h in hashes:
        print("{:6}".format(h), end="\t")
        for t in args.testsets:
            print("{:6}".format(results[(h,t)]), end="\t")
        print()
    # print(results)


if __name__ == "__main__":
    main()