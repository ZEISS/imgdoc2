// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace Imgdoc2cmd
{
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;

    internal abstract class CommandQueryForTilesBase:ICommand
    {
        private readonly CommandHelper commandHelper;

        protected CommandQueryForTilesBase()
        {
            this.commandHelper = new CommandHelper();
        }

        protected CommandHelper CommandHelper
        {
            get { return this.commandHelper; }
        }

        public void Execute(Options options)
        {
            this.commandHelper.OpenSourceDocument(options.SourceDocument);

            var reader2d = this.commandHelper.GetRead2d();

            DimensionQueryClause dimensionQueryClause = Utilities.CreateFromStringRepresentation(options.DimensionQuery);
            TileInfoQueryClause tileInfoQueryClause = new TileInfoQueryClause();
            tileInfoQueryClause.PyramidLevelConditionsModifiable.Add(new QueryClause() { ComparisonOperator = QueryComparisonOperator.Equal, Value = 0 });

            QueryOptions queryOptions = new QueryOptions
            {
                MaxNumbersOfResults = 100_000
            };

            var list = reader2d.Query(dimensionQueryClause, tileInfoQueryClause, queryOptions);

            this.ProcessTiles(list, options);
        }

        protected abstract void ProcessTiles(List<long> tiles, Options options);
    }
}
