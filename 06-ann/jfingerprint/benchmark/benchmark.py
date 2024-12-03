from jfingerprint.data.dataset import Dataset

class Benchmark:
    def __init__(self, gallery: Dataset, query: Dataset):
        """
        Initialize the Benchmark with gallery and query datasets.

        :param gallery: The gallery dataset.
        :type gallery: Dataset
        :param query: The query dataset.
        :type query: Dataset
        """
        self.gallery = gallery
        self.query = query

    def add_searcher(self, searcher_info: tuple):
        """
        Add a searcher to the benchmark.

        :param searcher_info: A tuple containing the searcher name, build kwargs, and search kwargs.
        :type searcher_info: tuple
        """
        pass

    def build(self, index: int):
        """
        Build the searcher at the specified index.

        :param index: The index of the searcher to build.
        :type index: int
        """
        pass

    def get_build_info(self) -> dict:
        """
        Get the build information of the searchers.

        :return: A dictionary containing the build information.
        :rtype: dict
        """
        pass

    def search(self, index: int):
        """
        Perform the queries using the searcher at the specified index.

        :param index: The index of the searcher to use for the queries.
        :type index: int
        """
        pass

    def get_result(self) -> dict:
        """
        Get the results of the benchmark.

        :return: A dictionary containing the results.
        :rtype: dict
        """
        pass